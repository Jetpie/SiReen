#!/usr/bin/env python
# encoding: utf-8
'''
run_compute_mp - compute results from EDA folder using
                 multiprocessing

@author:     recsearch

@copyright:  2014 Emar. All rights reserved.

@license:    license

@contact:    recsearch@emar.com

'''
import glob,os,time,logging,sys
import logging.config
from multiprocessing import Pool

########################################
# PATH CONFOGURATION
########################################
#HOME = '/home/bingqingqu/Employment/Emar/online/imagesearch_v2/'
HOME = '/data/rsworkspace/imagesearch/'
BIN = HOME + 'bin/'
DATA = HOME + 'res/result/'
RESULT = HOME + 'res/data/'
LOG = HOME + 'log/'
CONF = HOME + 'conf/'
LIB = HOME + 'lib/'
sys.path.append(LIB)
from recsearch.nearest_neighbour import KNearestSearch

########################################
# PATH CONFOGURATION
########################################
# number of process
PROCESSES = 10

########################################
# LOGGER CONFIGURATION
########################################

logging.config.fileConfig(CONF + 'logging.conf')
# create logger
logger = logging.getLogger('Main@PC')
logger_result = logging.getLogger('Result@PC')

def run_cmd(current_in_path):
    #  split to get current file
    current_in_file = current_in_path.split('/')[-1]
    logger.info('**********Current : %s **********' % current_in_file)
    try:
        logger_single = logging.getLogger("Atom@PC")
        kns = KNearestSearch(logger = logger_single,delimiter = "\t",use_sparse = True)
        kns.process(in_file = current_in_path, out_file = RESULT+current_in_file,
                  max_chunk_size = 2000)
        logger.info("********************SUCCEED********************")
        logger_result.info("%s"%current_in_file)
        return 0
    except:
        logger_result.error("%s - FAIL TO COMPUTE"%current_in_file )
        logger.error("********************FAIL********************")
        return -1
if __name__ == '__main__':

    logger.info("##############################Start##############################")
    start = time.time()

    # list all available files
    all_files = glob.glob(DATA + '*.txt')
    pool = Pool(PROCESSES)
    processes = pool._pool
    pool.map(run_cmd , all_files)

    pool.close()
    time.sleep(1)
    try:
        for worker in processes:
            print worker
            assert not worker.is_alive()
        logger.info('\tGarbage Collection Succeeded')
    except AssertionError:
        pool.terminate()
        sys.exit(0)

    logger.info('Elasped Time for All: %s' % str(time.time() - start))
    logger.info("##############################End##############################\n")
