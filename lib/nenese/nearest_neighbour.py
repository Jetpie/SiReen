#!/usr/bin/env python
# encoding: utf-8
'''
nearest_neighbour.py -- a general library to solve k nearest neighbour
                        searching probblem

@author:     recsearch

@copyright:  2014 Emar. All rights reserved.

@license:    commercial

@contact:    recsearch@emar.com

# Ver : 2.0.0
# Update Time : 2014-12-12
# Fix & Bugs :
# 1. integrate functions into a general library

# Ver : 2.0.1
# Update Time : 2014-12-16
# Fix & Bugs :
# 1. info index problem

# Ver : 2.1.0
# Update Time : 2014-12-17
# Fix & Bugs :
# 1. Adjust call structre of class 

# Ver : 2.1.1
# Update Time : 2014-12-17
# Fix & Bugs :
# 1. add memory check for chunk split

# Ver : 2.1.2
# Update Time : 2014-12-24
# Fix & Bugs:
# 1. modify the process coding pattern 
'''
from __future__ import division
import sys,os,logging,time,heapq
import numpy as np
from  scipy.spatial import distance
from scipy.sparse import csr_matrix, issparse, isspmatrix_csr
import array


__all__ = ['KNearestSearch']
__version__ = "2.1.2"
__date__ = '2014-12-12'
__updated__ = '2014-12-18'

### DEFINE VARIABLES ###
# 1g is approximately 1.074e+09 byte
ONE_GIB = 1.074 * 10**9
# limitation of memory usage
MEMORY_IN_GIB = 2


def _file_len(filename):
    """ get line numbers of the file """
    with open(filename) as f:
        for i, _ in enumerate(f):
            pass
    return i + 1

def _make_int_array():
    """Construct an array.array of a type suitable for scipy.sparse indices."""
    return array.array(str("i"))

def _assert_all_finite(X):
    """Like assert_all_finite, but only for ndarray."""
    X = np.asanyarray(X)
    if (X.dtype.char in np.typecodes['AllFloat'] and not np.isfinite(X.sum())
            and not np.isfinite(X).all()):
        raise ValueError("Input contains NaN, infinity"
                         " or a value too large for %r." % X.dtype)

def array2d(X, dtype=None, order=None, copy=False, force_all_finite=True):
    """Returns at least 2-d array with data from X"""
    if issparse(X):
        raise TypeError('A sparse matrix was passed, but dense data '
                        'is required. Use X.toarray() to convert to dense.')
    X_2d = np.asarray(np.atleast_2d(X), dtype=dtype, order=order)
    if force_all_finite:
        _assert_all_finite(X_2d)
    if X is X_2d and copy:
        X_2d = safe_copy(X_2d)
    return X_2d

def _atleast2d_or_sparse(X, dtype, order, copy, sparse_class, convmethod,
                         check_same_type, force_all_finite):
    if issparse(X):
        if check_same_type(X) and X.dtype == dtype:
            X = getattr(X, convmethod)(copy=copy)
        elif dtype is None or X.dtype == dtype:
            X = getattr(X, convmethod)()
        else:
            X = sparse_class(X, dtype=dtype)
        if force_all_finite:
            _assert_all_finite(X.data)
        X.data = np.array(X.data, copy=False, order=order)
    else:
        X = array2d(X, dtype=dtype, order=order, copy=copy,
                    force_all_finite=force_all_finite)
    return X

def atleast2d_or_csr(X, dtype=None, order=None, copy=False,
                     force_all_finite=True):
    """Like numpy.atleast_2d, but converts sparse matrices to CSR format

    Also, converts np.matrix to np.ndarray.
    """
    return _atleast2d_or_sparse(X, dtype, order, copy, csr_matrix,
                                "tocsr", isspmatrix_csr,
                                force_all_finite)


class KNearestSearch(object):
    """ K nearest neighbour search """
    def __init__(self, logger = None, k = 60, threshold = None, dimension = 500,
                 use_sparse = False, from_top_n = None, file_struct = None,
                 delimiter = ',', preprocesser = None, selecter = None,
                 sorter = None, constraint = None, sortby = None,
                 key_cols = None, formatter = "{0}:{1}"):
        # default logger is creating
        if not logger:
            self.logger = logging
        else:
            self.logger = logger

        self.k = k
        self.threshold = threshold
        self.dimension = dimension
        self.from_top_n = from_top_n
        self.use_sparse = use_sparse
        self.delimiter = delimiter
        if not file_struct:
            self.file_struct = {
                "category_id": 0,
                "merchant_id": 1,
                "url_md5" : 2,
                "weight" : 3,
                "commision" : 4,
                "state" : 5,
                "feature" : 6
                }
        else:
            self.file_struct = file_struct
        # currently, only equal logic is admitted
        if not constraint:
            self.constraint = {
                "commision" : ['1'],
                "state" : ['0']}
        else:
            self.constraint = constraint
        self.sortby = sortby
        if not key_cols:
            self.key_cols = ["url_md5", "merchant_id"]
        else:
            self.key_cols = key_cols
        self.formatter = formatter
        self.preprocesser = preprocesser
        self.selecter = selecter
        self.sorter = sorter
            
    def _build_selecter(self, selecter = None):
        """ build the candidate selecter"""
        if self.selecter is not None and callable(self.selecter):
            return self.selecter
        # default selecter
        # remove itself if it is valid
        # sort of bad syntax here according to stake overflow
        # but no obvious efficient problem 
        noop = lambda row, idx : np.delete(np.arange(row.size),idx) \
                                 if idx < row.size \
                                 else np.arange(row.size)
        if self.from_top_n:
            return lambda row, idx : heapq.nlargest(self.from_top_n, idx,
                        key = lambda x:row[x])
        else:
            return noop

            
    def _build_sorter(self, sorter = None):
        """ build the sort processer """
        if self.sorter is not None and callable(self.sorter):
            return self.sorter

        return lambda row, idx : heapq.nlargest(self.k, idx,
                        key = lambda x:row[x])

    def _check_pairwise_memory_usage(self, chunk_size, n):
        # only consider about the pairwise matrix memory usage
        return np.dtype(np.float64).itemsize * chunk_size * n  <= \
          MEMORY_IN_GIB * ONE_GIB

    def process(self, in_file, out_file, max_chunk_size = None, auto_adjust = True):
        """processing..."""
        start = time.time()
        self.fit(in_file,out_file)
        if not self._check_prerequsite():
            raise AttributeError("Lack of necessary object attributes")
        n_A = np.sum(self.A_flags)
        n_NA = self.n_samples - n_A
        self.logger.debug("[%s] Acvive | Inactive : %d | %d "
                        % (self.in_name,n_A, n_NA))
        self.logger.info("[%s] File Analysed (Elasped Time %f s)" \
                        % (self.in_name,(time.time() - start)))

        # build selecter, sorter
        self.selecter = self._build_selecter()
        self.sorter = self._build_sorter()

        # adjust max_chunk_size to fulfill memory limitation
        if max_chunk_size and auto_adjust:
            while not self._check_pairwise_memory_usage(max_chunk_size, n_A):
                max_chunk_size /= 2
            self.logger.debug("[%s] limitation: %d Gib | max_chunk_size: %d" \
                            % (self.in_name, MEMORY_IN_GIB, max_chunk_size))
                            
        # if max_chunk_limit not null
        if max_chunk_size:
            groups = np.array_split(np.where(self.A_flags)[0], \
                                    np.ceil((n_A + 1) / max_chunk_size)) + \
            np.array_split(np.where(~self.A_flags)[0], \
                           np.ceil((n_NA + 1) / max_chunk_size))
            # initialize base
            base = 0
            for i,idx in enumerate(groups):
                start = time.time()
                X_flags = np.zeros(self.n_samples,dtype = np.bool)
                X_flags[idx] = True
                self._partial_process(X_flags, base)
                self.logger.info("[%s] Processing chunk %d (Elasped Time %f s)" \
                                % (self.in_name,i,(time.time() - start)))
                base += idx.size
        else:
            # process active
            start = time.time()
            self._partial_process(self.A, 0)
            self.logger.info("[%s] Processing [Active Items] (Elasped Time %f s)" \
                            % (self.in_name,(time.time() - start)))
            # process inactive
            if n_NA > 0:
                start = time.time()
                self._partial_process(~self.A_flags,n_A)
                self.logger.info("[%s] Processing [Inactive Items] (Elasped Time %f s)" \
                                % (self.in_name,(time.time() - start)))
                                
        # # process A
        # start = time.time()
        # base = 0
        # if max_chunk_size and n_A > max_chunk_size:
        #     for i,idx in enumerate(np.array_split(np.where(self.A_flags)[0],
        #                                           n_A / max_chunk_size)):
        #         X_flags = np.zeros(self.n_samples,dtype = np.bool)
        #         X_flags[idx] = True
        #         self._partial_process(X_flags, base)
        #         base += idx.size
        # else:
        #     self._partial_process(self.A, 0)
        # self.logger.info("[%s]<Avail> Compute, Search, Write (Elasped Time %f s)" \
        #                  % (self.in_name,(time.time() - start)))
        # # stamp base index here
        # base = n_A
        # # check if partial processing is actived
        # if max_chunk_size and n_NA > max_chunk_size:

        #     # process Xs (by chunk)

        #     # chunk 1 to n
        #     for i,idx in enumerate(np.array_split(np.where(~self.A_flags)[0],
        #                                           n_NA / max_chunk_size)):
        #         start = time.time()
        #         X_flags = np.zeros(self.n_samples,dtype = np.bool)
        #         X_flags[idx] = True
        #         self._partial_process(X_flags, base)
        #         self.logger.info("[%s]<chunk-%d> Compute, Search, Write (Elasped Time %f s)" \
        #                         % (self.in_name,i+1,(time.time() - start)))
        #         base += idx.size
        # else:
        #     if n_NA > 0:
        #         start = time.time()
        #         # handle X
        #         X_flags = ~self.A_flags
        #         self._partial_process(X_flags,base)
        #         self.logger.info("[%s]<Unavail>Compute, Search, Write (Elasped Time %f s)" \
        #                         % (self.in_name,(time.time() - start)))
            
    def fit(self, in_file, out_file):
        """fit the input file """
        self.in_file = in_file
        self.in_name = in_file.split("/")[-1].split(".")[0]
        self.out_file = out_file
        
        # analyze the file structure
        self._file_analyzer(in_file)
        # get state-availiable item lists 
        A = self._read_partial_data(self.in_file, self.A_flags)
        self.A = atleast2d_or_csr(A)

        # finally, truncate the output content
        open(out_file, 'w').close()
        
    
    def _partial_process(self, X, base):
        # determine type of input X
        if X.dtype == bool:
            X = self._read_partial_data(self.in_file, X)
            X = atleast2d_or_csr(X)
        elif not X.dtype == np.float:
            raise ValueError("_partial_process param X type error")
            
        pairwise = self._compute(X)
        with open(self.out_file,"a") as out:
            for i,row in enumerate(pairwise.toarray()):
                # translate i
                i += base
                from_idx = self.selecter(row, i)
                if self.criteria:
                    top_idx = self.sorter(self.criteria, from_idx)
                else:
                    top_idx = self.sorter(row, from_idx)
                len(top_idx)
                out.write(self.keys[i] +
                          '\t' +
                          "\t".join([self.formatter.format(str(x),str(row[x])) \
                                     for x in top_idx]) +
                          '\n')
    def _check_prerequsite(self):
        """check pre-conditions are satisfied for processing"""""
        attr_check_list = ['in_file','A_flags','n_samples','keys',
                           'criteria','out_file','in_name']
        return all([hasattr(self,x) for x in attr_check_list])

    
    def _read_partial_data(self, infile, line_idx):
        """ read file by indicies"""
        # init sparse matrix construction
        j_indices = _make_int_array()
        indptr = _make_int_array()
        data = array.array(str("f"))
        
        # first element for row pointer is 0
        indptr.append(0)
        
        with open(infile, 'r') as file_buffer:
            for i,line in enumerate(file_buffer):
                if line_idx[i]:
                    features = line.strip().split(self.delimiter)[self.file_struct["feature"]]
                    # X.append(map(float,features.split(',')))
                    for j,elem in enumerate(map(float,features.split(','))):
                        if elem > 0:
                            j_indices.append(j)
                            data.append(elem)
                    indptr.append(len(j_indices))
        # some Python/Scipy versions won't accept an array.array:
        if j_indices:
            j_indices = np.frombuffer(j_indices, dtype=np.intc)
        else:
            j_indices = np.array([], dtype=np.int32)
        indptr = np.frombuffer(indptr, dtype=np.intc)
        data = np.frombuffer(data, dtype=np.float32)
        # construct csr format sparse matrix
        X = csr_matrix((data, j_indices, indptr),
                          shape=(len(indptr) - 1, self.dimension),
                          dtype=np.float64)
        if self.use_sparse:
            return X
        else:
            return X.toarray()

    def _file_analyzer(self,infile):
        """ pre-traverse the file to analyze the contents"""
        
        A_flags = []
        crit_idx = self.file_struct.get(self.sortby)
        criteria = []
        A_infos, UA_infos = [],[]
        with open(infile,'r') as file_buffer:
            for i,line in enumerate(file_buffer):
                # get contents by split
                contents = line.strip().split(self.delimiter)
                # check states of constraints
                if all([(contents[self.file_struct[key]] in self.constraint[key]) \
                        for key in self.constraint]):
                    A_flags.append(True)
                    A_infos.append("\t".join(contents[self.file_struct[key]] \
                                           for key in self.key_cols))
                    # index to sort the top N
                    if crit_idx:
                        criteria.append(contents[crit_idx])
                else:
                    A_flags.append(False)
                    UA_infos.append("\t".join(contents[self.file_struct[key]] \
                                           for key in self.key_cols))
        if np.sum(A_flags) < 1 :
            raise ValueError("NO valid item for recommendation")
        self.n_samples = i + 1
        if crit_idx:
            self.criteria = np.array(map(float,criteria))
        else:
            self.criteria = None
        self.keys = A_infos + UA_infos
        self.A_flags = np.array(A_flags)
        
        return 
    
    def _compute(self, X):
        return X * self.A.T
    
    def __repr__(self):
        print("KNearestSearch instance")
