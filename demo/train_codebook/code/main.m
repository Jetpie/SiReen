%% main controller for project
clear all;
close all;
clc;

% set path
addpath('01_train/');
% set path to vlfeat
VLFEATROOT = '/home/bingqingqu/user-libs/vlfeat-0.9.19/';
path_vlfeat = [VLFEATROOT 'toolbox/vl_setup'];
run(path_vlfeat);

%% Step 0 -  Parameters
params.category = 'caltech101';
params.home_path = '/home/bingqingqu/TAOCP/Github/SiReen/';
params.image_path = [params.home_path 'res/images/' params.category '/'];
params.descriptor_path = [params.home_path 'res/descriptors/' params.category '/'];
params.codebook_path = [params.home_path 'res/codebooks/' params.category '/'];
% set codebook dimension
params.N = 500;
params.sigma = 0;

% flags
train_descriptors_flag = 1;
train_codebook_flag = 1;

%% Step 0.5 - create directory structure
if ~exist(params.descriptor_path,'dir')
    mkdir(params.descriptor_path);
end
if ~exist(params.codebook_path,'dir')
    mkdir(params.codebook_path);
end

%% Step 1 - compute descriptors from images
if train_descriptors_flag
    if length(dir([params.image_path])) < 50
        error('Are you kiding me using less than 50 images for training codebook!?');
    end
    
    tic;
    compute_descriptors(params);
    disp('Descriptors computations done!');
    toc;
end


%% Step 2 - train codebook
if train_codebook_flag
    if length(dir([params.descriptor_path])) < 50
        error('no enough descriptors');
    end
    
    tic;
    train_codebook(params);
    disp('Codebook computation done!');
    toc;    
end

% remove path
rmpath('01_train/');