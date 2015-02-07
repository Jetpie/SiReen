#!/usr/bin/python
# encoding: utf-8
'''
autorun -- execute scripts and programs

@author:     Bingqing Qu

@license:    MIT

@contact:    sylar.qu@gmail.com

'''

import sys,os,logging,time
import logging.config
import ConfigParser

# SAFTY FLAG #
PID = time.strftime("_%d_%m_%Y")

########################################
# LOAD CONFIGURATION
########################################
# init config parser
config = ConfigParser.ConfigParser()
print sys.path[0] + "/bin/init.cfg"
config.read(sys.path[0] + "/bin/init.cfg")
# get localhost names
lh_names = config.get('Hostname','local').split(',')
# determine section
section = "Local" if os.uname()[1] in lh_names else "Server"
params = dict(config.items(section))

########################################
# LOGGER CONFIGURATION
########################################
logging.config.fileConfig(params['logcfg'])
# create logger
logger_main = logging.getLogger('main')
logger_fail = logging.getLogger("fail")

# Step 0
CMD_INIT = '/usr/bin/python ' + params['bin'] + 'initialize.py'
# Step 1
CMD_HIVE = '/usr/bin/python ' + params['bin'] + 'pull_hive.py'
# Step 2
CMD_FEATURE = '/usr/bin/python ' + params['bin'] + 'feature_mp.py'
# Step 3
CMD_SEARCH = '/usr/local/bin/python ' + params['bin'] + 'search_mp.py'
# Step 4
CMD_REDIS = '/usr/bin/python ' + params['bin'] + 'push_redis.py'

if __name__ == '__main__':
    start = time.time()
    logger_main.info("\tStart")

    status=os.system(CMD_INIT)
    if status==0:
        logger_main.info("Step 0 : Initialize Project (Elsaped Time: %f)" % \
                         (time.time() - start))
    else:
        logger_main.error("Step 0 Fail")
        logger_fail.error("Step 0 Fail")
        sys.exit(-1)

    start = time.time()
    status=os.system(CMD_HIVE)
    if status==0:
        logger_main.info("Step 1 : Download Data (Elsaped Time: %f)" % \
                         (time.time() - start))
    else:
        logger_main.error("Step 1 Fail")
        logger_fail.error("Step 1 Fail")
        sys.exit(-1)

    start = time.time()
    status=os.system(CMD_FEATURE)
    if status==0:
        logger_main.info("Step 2 : Feature Extraction (Elsaped Time: %f)" % \
                         (time.time() - start))
    else:
        logger_main.error("Step 2 Fail")
        logger_fail.error("Step 2 Fail")
        sys.exit(-1)

    start = time.time()
    status=os.system(CMD_SEARCH)
    if status==0:
        logger_main.info("Step 3 : Nearest Neighbor Search (Elsaped Time: %f)" % \
                         (time.time() - start))
    else:
        logger_main.error("Step 3 Fail")
        logger_fail.error("Step 3 Fail")
        sys.exit(-1)

    start = time.time()
    status=os.system(CMD_REDIS)
    if status==0:
        logger_main.info("Step 4 : Push Redis (Elsaped Time: %f)" % \
                         (time.time() - start))
    else:
        logger_main.error("Step 4 Fail")
        logger_fail.error("Step 4 Fail")
        sys.exit(-1)

    logger_main.info("\tEnd")
