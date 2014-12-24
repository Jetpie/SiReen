# encoding=utf-8
'''
utils.py - utils for convinience of daily manipulation

@author:     Bingqing Qu(recsearch)

@copyright:  2014 Emar. All rights reserved.

@license:    license

@contact:    recsearch@emar.com

# Ver : 1.01
# Update Time : 2014-11-29
# Fix & Bugs :
add get_category function for newest category download

'''

import sys,os,time
import codecs
try: reduce
except: from functools import reduce

__version__ = '1.01'
__all__ = ['CategoryManager']


def generate_corpus(l):
    corpus = l
    
    return corpus
def get_category(filename, host='221.122.127.131',port='3306',user='gwksearch',
                 password='r2bsHISIsNpp',database='show_case'):
    import mysql.connector
    cnx = mysql.connector.connect(host = host,
                                  port = port,
                                  user= user,
                                  password = password,
                                  database=database)
    cursor = cnx.cursor()
    query = ("SELECT cate_id,full_name "
             "FROM show_case.new_category "
             "WHERE level=3 and is_delete=0 "
             "ORDER BY cate_id ")
    cursor.execute(query)
    with codecs.open(filename,mode='w',encoding='utf-8') as outfile:
        outfile.write('\n'.join([','.join(map(unicode,pairs))
                                 for pairs in cursor]))
        
    cursor.close()
    cnx.close()    
    
# define category system 
class CategoryManager():

    def __init__(self,filepath):
        # file path 
        self._filepath = filepath
        # init category as list of dict
        self._category = [{},{},{}]

        # load file content of .csv (typically)
        self._load_file(self._filepath)
        
    def __repr__(self):
        '''
        Good representation of class information
        '''
        return "CategoryManager Instance Readed from [%s]" % self._filepath

    def _load_file(self,filename):
        start = time.time()
        
        # initialize for build inverted index
        self._toc, corpus = {},set()
        with codecs.open(filename,mode='r',encoding='utf-8') as infile:
            
            for line in infile:
                # comma separated as it is .csv
                id,name = line.strip().split(',')
                # handle 3rd level
                names = name.split('|')
                tokens = []
                for part in names:
                    tokens+= part.split(u'\u3001')
                # tokens = generate_corpus([part.split(u'\3001') for part in names])
                corpus |= set(tokens)
                
                self._toc.setdefault(id,tokens)
                
                self._category[2].setdefault(id,names)
                # handle 2nd level
                if not id[:7] in self._category[1]:
                    self._category[1].setdefault(id[:7],names[:2])
                # handle 1st level
                if not id[:4] in self._category[0]:
                    self._category[0].setdefault(id[:4],names[:1])

        # generate inverted index
        self._invindex={token:set(id for id,tokens in self._toc.items() if token in tokens)\
                        for token in corpus}
        
        print "Elasped time for loading: %f" %(time.time()-start)
        return
    
    def overview(self):
        '''
        print overview of current category system
        '''
        print("*"*80)
        print("-Category System Overview-")
        print("+------------------------------+----------+")
        print("|Num 1st level category        |%10d|"%len(self._category[0]))
        print("|Num 2nd level category        |%10d|"%len(self._category[1]))
        print("|Num 3rd level category        |%10d|"%len(self._category[2]))
        print("+------------------------------+----------+")
        print("*"*80)
        return
    
    def get_all_ids(self,level=3):
        return self._category[level-1].keys()
        
    def search_by_id(self,id,delimiter='|'):
        '''
        return category name by id
        id can indicate a 1st level id/2nd level id/full id
        '''
        id = str(id)
        if len(id) == 4:
            category = self._category[0]
        elif len(id) == 7:
            category  = self._category[1]
        elif len(id) == 10:
            category = self._category[2]
        else:
            print("Warning: wrong category id length for searching.")
            return None
        # if not exist in category
        if not id in category:
            print("Warning: no category_id" + id +
                  " in current category system")
            return None
        return delimiter.join(category[id])
    
    def search_by_words(self,query):
        # print len(self._invindex)
        query = [unicode(x,'utf-8') for x in query]
        return reduce(set.intersection,
                      (self._invindex[term] for term in query \
                       if term in self._invindex),
                      set(self._toc.keys()))
