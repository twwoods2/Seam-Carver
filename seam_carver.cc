#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
const ImagePPM& SeamCarver::GetImage() const { 
  return image_;
}
int SeamCarver::GetHeight() const { 
  return height_;
}

  // returns the image's width
  int SeamCarver::GetWidth() const { // not implemented
    return width_;
  }

  // returns the energy of the pixel at row col in image_
  int SeamCarver::GetEnergy(int row, int col) const { // not implemented
    Pixel up = Pixel{};
    Pixel down = Pixel{};
    Pixel left = Pixel{};
    Pixel right = Pixel{};
    if (row == 0) { //if Pixel on the ceiling
       up = image_.GetPixel(height_ - 1, col);
    } else {
       up = image_.GetPixel(row - 1, col);
    }
    if (row + 1 == height_) { // if Pixel is on the floor
       down = image_.GetPixel(0, col); 
    } else {
       down = image_.GetPixel(row + 1, col);
    }
    //Pixel down = image_.GetPixel(row - 1, col);
    if (col == 0) { // if Pixel is on far left
       left = image_.GetPixel(row, width_ - 1);
    } else {
       left = image_.GetPixel(row, col - 1);
    }
    if (col == width_ - 1) { // if Pixel is on far right
       right = image_.GetPixel(row, 0);
    } else {
       right = image_.GetPixel(row, col + 1);
    }
    //put the left and right into helper and up and down.
    int row_col_energy = DeltaColorValue(up, down) + DeltaColorValue(left, right);
    return row_col_energy;
  }
  int SeamCarver::DeltaColorValue(Pixel dominant, Pixel latter) const{
    int red = dominant.GetRed() - latter.GetRed();
    int green = dominant.GetGreen() - latter.GetGreen();
    int blue = dominant.GetBlue() - latter.GetBlue(); 
    //square everything
    red = red * red;
    green = green * green;
    blue = blue * blue;
    return red + blue + green;
  }


  // returns the horizontal seam of image_ with the least amount of
  // energy
  //
  // the ith int in the returned array corresponds to which row at
  // col i is in the seam. example:
  //
  //    | x |   |
  // ---+---+---+---
  //  x |   | x |
  // ---+---+---+---
  //    |   |   | x
  // returns {1, 0, 1, 2}
  int* SeamCarver::GetHorizontalSeam() const { // not implemented
    int** horizontal = Empty2dArray(height_, width_);
    for (int row = 0; row < height_; row++) {
      horizontal[row][width_ - 1] = GetEnergy(row, width_ - 1);
    }
    for (int col = width_ - 2; col >= 0; col--) {
      for (int row = 0; row < height_; row++) {
        int fillout = GetEnergy(row, col);
        int forward = horizontal[row][col + 1];
        int southeast = INT32_MAX;
        int northeast = INT32_MAX;
        if (row != 0) {
          northeast = horizontal[row - 1][col + 1];
        }
        if (row != height_ - 1) {
          southeast = horizontal[row + 1][col + 1];
        }
        if (forward <= southeast && forward <= northeast) {
          horizontal[row][col] = fillout + forward;
        }
        if (northeast < forward && northeast <= southeast) {
          horizontal[row][col] = fillout + northeast;
        }
        if (southeast < forward && southeast < northeast) {
          horizontal[row][col] = fillout + southeast;
        }
      }
    }
    int min_index = 0;
    for (int row = 1; row < height_; row++) {
      if (horizontal[row][0] < horizontal[min_index][0]) {
        min_index = row;
      }
    }
    int* hor_path = HorizontalHelper(horizontal, min_index);
    for (int row = 0; row < height_; row++) {
     delete[] horizontal[row];
    }
    delete[] horizontal;
    return hor_path;
  }
  
  int* SeamCarver::HorizontalHelper(int** arr, int index_start) const {
    int* hor_ptr = new int[width_];
    hor_ptr[0] = index_start;
    for (int col = 1; col < width_; col++) {
      int forward = arr[index_start][col];
      int northeast = INT32_MAX;
      int southeast = INT32_MAX;
      if (index_start != height_ - 1) {
        southeast = arr[index_start + 1][col];
      }
      if (index_start != 0) {
        northeast = arr[index_start - 1][col];
      }
      if (forward <= northeast && forward <= southeast) {
        hor_ptr[col] = index_start;
      }
      if (northeast < forward && northeast <= southeast) {
        index_start--;
        hor_ptr[col] = index_start;
      }
      if (southeast < forward && southeast < northeast) {
        index_start++;
        hor_ptr[col] = index_start;
      }
    }
    return hor_ptr;
  }
  int* SeamCarver::GetVerticalSeam() const {
  int** vert_energy = Empty2dArray(height_, width_); // populate array with least energy path
  for (int col = 0; col < width_; col++) {
    vert_energy[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }
  for (int row = height_ - 2; row >= 0; row--) { // up two (must be able to evaluate the array below)
    for (int col = 0; col < width_; col++) { // left to right
      int energy_path_comp = GetEnergy(row, col);
      int vert_down = vert_energy[row + 1][col];
      int vert_left = INT32_MAX;
      int vert_right = INT32_MAX;
      if (col != width_ - 1) {
        vert_right = vert_energy[row + 1][col + 1];
      }
      if (col != 0) {
        vert_left = vert_energy[row + 1][col - 1];
      }
      if (vert_down <= vert_right && vert_down <= vert_left) {
        vert_energy[row][col] = energy_path_comp + vert_down;
      }
      if (vert_left < vert_down && vert_left <= vert_right) {
        vert_energy[row][col] = energy_path_comp + vert_left;
      }
      if (vert_right < vert_down && vert_right < vert_left) {
        vert_energy[row][col] = energy_path_comp + vert_right;
      }
    }
  }
  int* to_return = FindMinValPath(vert_energy);
  for (int row = 0; row < height_; row++) {
    delete[] vert_energy[row];
  }
  delete[] vert_energy;
  return to_return;
  }
  
  int* SeamCarver::FindMinValPath(int** energy_arr) const {
  int min_index = 0;
  for (int col = 1; col < width_; col++) {
        if (energy_arr[0][col] < energy_arr[0][min_index]) {
          min_index = col;
      }   
    }
    int* path_ptr = new int[height_];
    path_ptr[0] = min_index;
    for (int row = 1; row < height_; row++) {
        int vert = energy_arr[row][min_index];
        int left = INT32_MAX;
        int right = INT32_MAX;
        if (min_index != 0) {
          left = energy_arr[row][min_index - 1];
        }
        if (min_index != width_ - 1) {
          right = energy_arr[row][min_index + 1];
        }
        if (vert <= right && vert <= left) {
          path_ptr[row] = min_index;
        }
        if (left < vert && left <= right) {
          min_index--;
          path_ptr[row] = min_index;
        }
        if (right < vert && right < left) {
          min_index++;
          path_ptr[row] = min_index;
        }
      }
  return path_ptr;
}
    
int** SeamCarver::Empty2dArray(int num_rows,int num_cols) const{
  int** arr = new int*[num_rows];
  for (int i = 0; i < num_rows; i++) {
    arr[i] = new int[num_cols];
  }
  return arr;
}
  
  
  void SeamCarver::RemoveHorizontalSeam() {
    auto** new_pixel_array = new Pixel*[height_ - 1];
    for (int row = 0; row < (height_ - 1); row++) {
      new_pixel_array[row] = new Pixel[width_];
    }
    int* small_vert = GetHorizontalSeam();
    for (int col = 0; col < width_; col++) {
      for (int row = 0; row < height_ - 1; row++) {
        if (row <  small_vert[col]) {
          new_pixel_array[row][col] = image_.GetPixel(row, col);
        } else {
          new_pixel_array[row][col] = image_.GetPixel(row + 1, col);
        }
      }
    }
    //Create new PPM
    image_.~ImagePPM();
    image_.SetPixel(new_pixel_array, width_, height_ - 1);
    delete[] small_vert;
    height_--;
  }

  
  void SeamCarver::RemoveVerticalSeam() {
    auto** new_pixel_array = new Pixel*[height_];
    for (int row = 0; row < height_; row++) {
      new_pixel_array[row] = new Pixel[width_ - 1];
    }
    int* small_vert = GetVerticalSeam();
    for (int row = 0; row < height_; row++) {
      int new_index_col = 0;
      for (int col = 0; col < width_; col++) {
        if (col < small_vert[row] || col > small_vert[row]) {
          new_pixel_array[row][new_index_col] = image_.GetPixel(row, col);
          new_index_col++;
        } else if (col == small_vert[row]) {
          continue;
        }
      }
    }
    //Create new PPM
    image_.~ImagePPM();
    image_.SetPixel(new_pixel_array, width_ - 1, height_);
    delete[] small_vert;
    width_--;
  }




