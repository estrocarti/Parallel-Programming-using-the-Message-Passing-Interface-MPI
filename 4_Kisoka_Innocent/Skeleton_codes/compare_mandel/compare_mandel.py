import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

# Function to compare two images
def compare_images(file1, file2):
    # Load images
    image1 = np.array(Image.open(file1))
    image2 = np.array(Image.open(file2))
    
    # Check if the images have the same dimensions
    if image1.shape != image2.shape:
        print("The images have different dimensions.")
        return
    
    # Compare pixel-by-pixel
    if np.array_equal(image1, image2):
        print("The images are identical.")
    else:
        print("The images are slightly different.")
        
        # Show the differences visually
        diff = np.abs(image1 - image2)
        plt.imshow(diff, cmap="hot")
        plt.title("Differences between Sequential and Parallel Output")
        plt.colorbar(label="Difference Intensity")
        plt.savefig("Performance Comparison of Sequential and MPI-based Mandelbrot Computations")
        plt.show()

# Function to calculate numerical error between images
def calculate_error(file1, file2):
    image1 = np.array(Image.open(file1))
    image2 = np.array(Image.open(file2))

    # Calculate the mean squared error between the two images
    error = np.mean((image1 - image2) ** 2)
    print(f"Mean Squared Error between the two images in Pixels: {error}")
    
    return error

# Run comparison and error calculation
file_sequential = '/home/kisoki/Skeleton_codes/mandel_seq/mandel_seq.png' 
file_parallel = '/home/kisoki/Skeleton_codes/mandel/mandel.png'

# Note :
# Please update the file paths Above to match your local directory structure before running the program.
# The current paths are specific to the author's setup and may not exist on your system.
# Update the paths for 'file_sequential' and 'file_parallel' as needed to avoid errors.



# Visual comparison
compare_images(file_sequential, file_parallel)

# Numerical comparison
error = calculate_error(file_sequential, file_parallel)

# Check if the error is within acceptable tolerance
tolerance = 0.5
if error < tolerance:
    print("The parallel program results are correct.")
else:
    print("There is a significant discrepancy between the parallel and sequential results.")
