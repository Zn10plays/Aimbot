load("camparams.mat", "cameraParams")

% camera intrinsics
[intrinsicMatrix, distortionCoeffs] = cameraIntrinsicsToOpenCV(cameraParams)

fileID = fopen('camera_calib.yaml','w');

fprintf(fileID, '%%YAML:1.0\n');
fprintf(fileID, 'camera_matrix: !!opencv-matrix\n');
fprintf(fileID, '   rows: 3\n');
fprintf(fileID, '   cols: 3\n');
fprintf(fileID, '   dt: d\n');
fprintf(fileID, '   data: [%f, %f, %f, %f, %f, %f, %f, %f, %f]\n', intrinsicMatrix');

fprintf(fileID, 'distortion_coefficients: !!opencv-matrix\n');
fprintf(fileID, '   rows: 1\n');
fprintf(fileID, '   cols: %d\n', length(distortionCoeffs));
fprintf(fileID, '   dt: d\n');
fprintf(fileID, '   data: [%s]\n', num2str(distortionCoeffs, '%.10f, '));
fclose(fileID);
