#!/usr/bin/python2
# encoding: utf-8
'''
search_nearest.py

@author: Bingqing Qu

@license: GPL

@contact: sylar.qu@gmail.com

'''
from sklearn.neighbors import NearestNeighbors
import numpy as np
import time,sys
try:
    import cv2
except:
    print("no cv2 found")
# edit root for absolute if any path problem encountered
SIREEN_ROOT = "../../"
CURRENT = "caltech101"
labels = []
features = []
start = time.time()
# read llc result file
with open(SIREEN_ROOT + "res/llc/"+CURRENT+ ".txt","r") as infile:
    for line in infile:
        contents = line.strip().split("\t")
        try:
            feature = map(float,contents[-1].split(","))
        except:
            continue
        features.append(feature)
        labels.append(contents[0].split("/")[-1])

print("Elasped time for reading data : %f"%(time.time() - start))

features = np.array(features,dtype = np.float)
# search nearest neighbor
start = time.time()
kd_tree = NearestNeighbors(n_neighbors = 10, algorithm = "brute")
kd_tree.fit(features)

print("Elasped time for kd-tree build : %f"%(time.time() - start))

start = time.time()
distances,idxs = kd_tree.kneighbors(features)

idxs_buf = []
for distance, idx in zip(distances, idxs):
    idx = list(idx[distance < 0.7])
    idxs_buf.append(idx)
idxs = idxs_buf
print("Elasped time for data search : %f"%(time.time() - start))

# write result
start = time.time()
tp = 0.0
b = 0.0
with open(SIREEN_ROOT + "res/result/"+CURRENT+".txt","w") as outfile:
    for idx in idxs:
        # not a scalar
        if len(idx) > 1:
            prefix = labels[idx[0]].split("_")[0]
            # idx = np.random.choice(591,10,replace=False)
            for i in idx[1:]:
                b +=1
                if labels[i].split("_")[0] == prefix:
                    tp+=1
            print([labels[n] for n in idx])
            outfile.write(labels[0] + "\t" + ",".join(map(str,idx[1:])))
print(features.shape)
print "b",b
print("accuracy %f"%(tp/b))
print("Elasped time for write file : %f"%(time.time() - start))

for idx in idxs:
    images = [cv2.resize(cv2.imread(SIREEN_ROOT +  \
                'res/images/'+CURRENT+'/' + labels[i]), \
                (64,64),interpolation=cv2.INTER_CUBIC) for i in idx]
    result_img = np.concatenate(images,axis=1)

    cv2.imshow("search result",result_img)
    k = cv2.waitKey(0)& 0xFF
    if k == 27:         # wait for ESC key to exit
        cv2.destroyAllWindows()
        sys.exit(-1)
cv2.destroyAllWindows()
