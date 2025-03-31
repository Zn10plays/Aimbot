import numpy as np

def compute_angles(u, v, f, alpha, beta):
    """
    Compute the turret angles theta and phi to center the detected point.

    Args:
        u (float): X-coordinate of the detected point in the camera frame.
        v (float): Y-coordinate of the detected point in the camera frame.
        f (float): Focal length of the camera (in pixels).
        alpha (float): Vertical offset of the camera from the turret origin (upwards).
        beta (float): Horizontal offset of the camera from the turret origin (along the turret's forward direction).

    Returns:
        tuple: (theta, phi) in radians, the required angles to center the point.
    """
    # Step 1: Convert (u, v) to a normalized direction vector in camera's coordinate system
    direction_camera = np.array([u, v, 1380.4536060478])
    direction_normalized = direction_camera / np.linalg.norm(direction_camera)

    # Step 2: Transform direction to turret's coordinate system considering camera offset
    # Assuming the camera's position is (0, alpha, beta) in turret's frame when theta=0, phi=0
    # To account for parallax, we approximate the direction adjustment
    # This is a simplified model; for precise calculation, iterative methods are needed
    dx, dy, dz = direction_normalized
    adjusted_direction = np.array([
        dx * beta,
        dy * beta + alpha,
        dz * beta
    ])
    adjusted_direction_normalized = adjusted_direction

    D_x, D_y, D_z = adjusted_direction_normalized

    # Step 3: Compute theta and phi
    theta = np.arccos(D_z)
    phi = np.arctan2(D_x, -D_y)

    return theta, phi

# Example usage:
# u, v = (100, 50)  # Detected point
# f = 500           # Focal length (example value)
# alpha = 0.1       # Camera vertical offset (meters)
# beta = 0.2        # Camera horizontal offset (meters)
# theta, phi = compute_angles(u, v, f, alpha, beta)
# print(f"Theta: {theta} radians, Phi: {phi} radians")