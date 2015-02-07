%% train codebook
function train_codebook(params)
%% get parameters
category = params.category;
descriptor_path = params.descriptor_path;
codebook_path = params.codebook_path;
% N codebooks
N = params.N;

%% initializations
descriptors = dir([descriptor_path '*.mat']);
n_descriptors = length(descriptors);


% all descriptors
% dimmension should be M * 128
data = [];
for i = 1 : min(n_descriptors,4000)
    try
    load([descriptor_path ,descriptors(i).name]);
    catch
        continue;
    end
    data = [data; features.data];
end
[n_data,~] = size(data);
% permutation to select N centers for initialization
% rng for debug
% rng(1);
perm = randperm(n_data,N);
% N * 128
centers = data(perm, :);

%% K-means
disp(['begin k-means:' num2str(n_data)]);
data = data';
centers = centers';

% The k-means algorithm.
% kmeans seed
[~,L] = max(bsxfun(@minus,2*real(centers'*data),dot(centers,centers,1).'),[],1);
L1 = 0;
while any(L ~= L1)
    L1 = L;
    for i = 1:N, l = L==i; centers(:,i) = sum(data(:,l),2)/sum(l); end
    [~,L] = max(bsxfun(@minus,2*real(centers'*data),dot(centers,centers,1).'),[],1);
end

dictionary = centers';

% save the settings of descriptor
save([codebook_path '\cb' category '.mat'], 'dictionary');
dlmwrite([codebook_path '\cb' category '.txt'],dictionary,'delimiter',',');
end