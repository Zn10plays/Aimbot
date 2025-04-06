import cv2
import numpy as np
import yaml

cfg = yaml.safe_load(open('../cfg.yaml', 'r'))

# ======= Load calibration data (from previous calibration) =======
# You would typically load this from a file, but here we assume you already have them
# Example dummy values (REPLACE these with your actual calibration output)
camera_matrix = np.array(cfg['camera_matrix'], dtype=np.float32)
dist_coeffs = np.array(cfg['distortion_coefficients'], dtype=np.float32)  # example distortion coefficients

# ======= Load your test image =======
image = cv2.imread('./calibration_images/image_003.jpg')  # replace with your image path

# ======= Undistort image =======
h, w = image.shape[:2]
new_camera_matrix, roi = cv2.getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, (w,h), 1, (w,h))

# Undistort
undistorted_img = cv2.undistort(image, camera_matrix, dist_coeffs, None, new_camera_matrix)

# Crop if needed (optional)
x, y, w, h = roi
undistorted_img = undistorted_img[y:y+h, x:x+w]

# Resize original to match undistorted (for side-by-side)
image_resized = cv2.resize(image, (undistorted_img.shape[1], undistorted_img.shape[0]))

# ======= Side-by-side comparison =======
comparison = np.hstack((image_resized, undistorted_img))

# Show the result
cv2.imshow('Original (Left) vs Undistorted (Right)', comparison)
cv2.waitKey(0)
cv2.destroyAllWindows()
