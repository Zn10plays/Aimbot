checkerboard(9,6);
imwrite(ans, 'checkerboard.png');

save("camera_cfg.mat", "cameraParams")

% read as such cameraParams.Intrinsics
% or cameraParams.Intrinsics.K

writematrix(cameraParams.Intrinsics.K, "intrinsics.csv")