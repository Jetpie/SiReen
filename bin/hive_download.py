#! /usr/bin/env python
import sys,os,logging,time,glob
import logging.config
# SAFTY FLAG #
PID = time.strftime("_%d_%m_%Y")

##################################################
# PATH CONFIGURATION
##################################################

BIN_DIR= sys.path[0]+"/"
HOME_DIR= BIN_DIR.replace('bin/','')
LOADING_FILE= HOME_DIR+"res/sql" + PID + ".txt"
RES_DIR = HOME_DIR + 'res/'
MIN_ROW_SIZE= 4000000
CONFIGURE = HOME_DIR + 'conf/category.conf'

# LOGGER CONFIGURATION #
logging.config.fileConfig('/data/rsworkspace/imagesearch/conf/logging.conf')
# create logger
logger = logging.getLogger('Main@Sql')

with open(CONFIGURE,'r') as cat_config:
    cats_query = ','.join(["'"+cat.strip()+"'" for cat in cat_config])

HIVE_QL="select concat_ws(cast(unhex('1') as string),cast(category_id as string),url_md5,img_url, " \
"cast(merchant_id as string),cast(product_weight as string),cast(commosion as string),cast(prod_state as string)) " \
"from gwk.product_offline " \
"where length(img_url)>5 and product_weight>=20 " \
"and (prod_state=0 OR prod_state=2 OR prod_state=3 OR prod_state=8) " \
"and substr(category_id,0,10) in("+cats_query+");"

logger.debug(HIVE_QL)

logger.debug('HOME_DIR='+HOME_DIR)
logger.debug('BIN_DIR='+BIN_DIR)

logger.info('########################### start #################################')


#first to download data file from hadoop
logger.info('####Download data of products to the file '+LOADING_FILE+'.Begin.....')

status=os.system("touch "+LOADING_FILE)

status=os.system("chmod a+rw "+LOADING_FILE)

if status==0:
        logger.info("chmod a+rw"+LOADING_FILE+" finished.")
else:
        logger.error("chmod a+rw"+LOADING_FILE+" failed.")
	sys.exit(1)

status=os.system("su - hadoop-cdh -c \' hive -S -e \"" + HIVE_QL + "\" > "+LOADING_FILE+"\'")

if status==0:
        logger.info("Download data to "+LOADING_FILE+" finished.")
else:
        logger.error("Fail to download data.")
	sys.exit(1)


rows=os.popen("wc -l "+LOADING_FILE,'r').readlines()[0]

if rows<MIN_ROW_SIZE:
        logger.error("the size of rows of the file "+LOADING_FILE+\
                     " is less than the threshold of "+MIN_ROW_SIZE+".")
	sys.exit(1)
else:
        logger.info("the size of rows of the file "+LOADING_FILE+" equal to "+str(rows)+".")

logger.info('Download data of products to the file '+LOADING_FILE+'.End.....')


if len(glob.glob(RES_DIR + 'sqldata/*')) > 0:
    logger.error("Please Clear the sqldata folder first!")
    sys.exit(-1)
with open(LOADING_FILE,'r') as sql:
    for line in sql:
        cur_cat = line.split('\x01')[0]
        with open(RES_DIR+'sqldata/' + cur_cat + '.txt','a') as out:
            out.write(line)


logger.info('########################### finished #################################')
