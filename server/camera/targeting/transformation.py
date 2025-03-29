import numpy as np

def compute_turret_angles(u, v, camera_matrix, alpha, beta, theta_current, phi_current):
    """
    Compute the turret angles (theta, phi) required to center a target (u, v) in the camera frame.
    
    Parameters:
    - u, v: Target pixel coordinates in the camera image.
    - camera_matrix: 3x3 intrinsic camera matrix.
    - alpha: Vertical offset of the camera from the turret origin.
    - beta: Forward offset of the camera from the turret origin.
    - theta_current, phi_current: Current turret angles.

    Returns:
    - (theta_desired, phi_desired): The new turret angles.
    """
    # Camera intrinsics (inverse transformation)
    uv1 = np.array([u, v, 1.0])  # Homogeneous coordinates
    direction_cam = np.linalg.inv(camera_matrix) @ uv1
    direction_cam /= np.linalg.norm(direction_cam)  # Normalize to unit vector

    # Camera to turret transformation
    x_c, y_c, z_c = direction_cam

    # Adjust for camera position offset (translation)
    x_t = x_c + beta
    y_t = y_c + alpha
    z_t = z_c  # Camera is assumed to be aligned with turret forward

    # Compute new angles in turret space
    theta_desired = np.arctan2(x_t, z_t)  # Yaw (horizontal)
    phi_desired = np.arctan2(y_t, np.sqrt(x_t**2 + z_t**2))  # Pitch (vertical)

    # Compute required adjustments
    delta_theta = np.degrees(theta_desired - theta_current)
    delta_phi = np.degrees(phi_desired - phi_current)

    return theta_desired, phi_desired, delta_theta, delta_phi

# Example values
camera_matrix = np.array([[1394.77415450476,0,508.291453466793],  # fx, 0, cx
                          [0,1380.4536060478,209.447535174643],  # 0, fy, cy
                          [0, 0, 1]])     # 0, 0, 1


u, v = 350, 260  # Target pixel coordinates
alpha, beta = 0.1, 0.2  # Camera offset from turret origin
theta_current, phi_current = np.radians(10), np.radians(-5)  # Current turret angles

# Compute turret angles
theta_new, phi_new, d_theta, d_phi = compute_turret_angles(u, v, camera_matrix, alpha, beta, theta_current, phi_current)
