%% compute dsift descriptors 
function compute_descriptors(params)
%% get parameterss
image_path = params.image_path;
descriptor_path = params.descriptor_path;
sigma = params.sigma;
images = dir([image_path '*']);
num = length(images);
n_error = 0;
for i = 1:num
    disp(['computation progress: ' num2str(i) '/' num2str(num)]);
    try
        I = rgb2gray(imresize(imread([image_path images(i).name]),[128,128]));
    catch
        n_error = n_error + 1;
        continue
    end
    features = compute_dsift_SPRS(I,sigma);
    save ([descriptor_path,'desc_',images(i).name(1:end-4) '.mat'], 'features');
end
disp(['n_error: ' num2str(n_error)]);

end

% compute dsift
function features = compute_dsift_SPRS(I,sigma)

if length(size(I)) ~= 1
    I = I(:,:,1);
end


I = im2single(I);
[hgt, wid] = size(I);
if sigma ~=0
    % sigma = 1.2;
    I = vl_imsmooth(I, sigma) ;
end
% I = double(I);
binsize =16;
[positions, siftArr] = vl_dsift(I,'size',binsize,'fast','step',8);
siftArr = double(siftArr)';
siftArr = normalize_sift(siftArr);
features.data = siftArr;
features.x = positions(1,:);
features.y = positions(2,:);
features.width = wid;
features.height = hgt;

end

% normalize
function sift_arr = normalize_sift(sift_arr)
% normalize SIFT descriptors (after Lowe)

% find indices of descriptors to be normalized (those whose norm is larger than 1)
tmp = sqrt(sum(sift_arr.^2, 2));
normalize_ind = find(tmp > 1);

sift_arr_norm = sift_arr(normalize_ind,:);
sift_arr_norm = sift_arr_norm ./ repmat(tmp(normalize_ind,:), [1 size(sift_arr,2)]);

% suppress large gradients
sift_arr_norm(find(sift_arr_norm > 0.2)) = 0.2;

% finally, renormalize to unit length
tmp = sqrt(sum(sift_arr_norm.^2, 2));
sift_arr_norm = sift_arr_norm ./ repmat(tmp, [1 size(sift_arr,2)]);

sift_arr(normalize_ind,:) = sift_arr_norm;

end