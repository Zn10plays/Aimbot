import cv2
import os

# Set the folder where images will be saved
SAVE_DIR = "calibration_images"
os.makedirs(SAVE_DIR, exist_ok=True)  # Create folder if it doesn't exist

# Open webcam
cap = cv2.VideoCapture(1)  # Change to 1 or other index if using an external camera

if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

img_counter = 0  # Image counter for filenames

print("Press SPACE to capture an image. Press 'q' to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break

    # Display the frame
    cv2.imshow("Camera Feed", frame)

    # Wait for key press
    key = cv2.waitKey(1)

    if key == ord(' '):  # Spacebar to capture
        img_filename = os.path.join(SAVE_DIR, f"image_{img_counter:03d}.jpg")
        cv2.imwrite(img_filename, frame)
        print(f"Saved: {img_filename}")
        img_counter += 1  # Increment counter

    elif key == ord('q'):  # Press 'q' to quit
        print("Closing camera...")
        break

# Release resources
cap.release()
cv2.destroyAllWindows()
