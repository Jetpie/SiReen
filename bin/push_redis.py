#!/usr/bin/env python
# encoding: utf-8
'''
run_upload - push computed result to redis

@author:     bingqing qu

@license:    MIT

@contact:    sylar.qu@gmail.com

'''
import glob,os,time
import json
import logging
import logging.config
from rediscluster import RedisCluster
########################################
# PATH CONFOGURATION
########################################

# init config parser
config = ConfigParser.ConfigParser()
config.read("init.cfg")
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
logger_redis = logging.getLogger('redis')
logger_fail = logging.getLogger("fail")

########################################
# REDIS CONFIGURATION
########################################
redis_key_prefix = config.get('Redis','prefix')
with open(params["conf"] + config.get('Redis','json'),"r") as jsonfile:
    startup_nodes = json.load(jsonfile, encoding="utf-8")
redis_clients = RedisCluster(startup_nodes=startup_nodes, decode_responses=True)
EXPIRE_THREE_DAYS = config.getint('Redis','expire') * 24 * 60 *60

def upload(source_file):
    start = time.time()
    # get name sequentially
    with open(source_file,'r') as file:
        names = [",".join(line.split('\t')[0:2]) for line in file]

    count = 0
    # read and upload key & values
    with open(source_file,'r') as file:
        for line in file:
            tokens = line.strip().split('\t')
            # key: prefix + md5
            key = redis_key_prefix + tokens[0]
            # value: md5,merchant_id,distance\1 ...
            value = '\1'.join(map(lambda pair: \
                                    names[int(pair[0])] + ',' + pair[1] , \
                                    [pair.split(':') for pair in tokens[2::]]))
            #upload to redis
            redis_clients.set(key,value)
            redis_clients.expire(key,EXPIRE_THREE_DAYS)

            if count % 1000 == 0:
                # print key
                # print value
                print(count)
                # print("%s : %d"%(key, redis_clients.ttl(key)))
            count +=1

    return 0


if __name__ == "__main__":

    logger_redis.info("\tStart")
    start = time.time()

    # list all available files
    all_files = glob.glob(RESULT + '*.txt')

    num_of_files = len(all_files)
    for current_in_path in all_files:
        current_in_file = current_in_path.split('/')[-1]
        status = upload(current_in_path)
        if status==0:
            logger_redis.info('[%s] Done' % current_in_file)
        else:
            logger_redis.error('[%s] Fail' % current_in_file)
            logger_fail.error('[%s](Redis) Fail' % current_in_file)

    logger_redis.info('\tEnd (Elasped Time : %s)\n' % str(time.time() - start))
