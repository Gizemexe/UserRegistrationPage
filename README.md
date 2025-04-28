# Customer Registration GUI Application

This project is a **simple C++ GUI application** developed using the `ICBYTES`, `ic_media.h`, and `icb_gui.h` libraries. It is designed to demonstrate user registration, photo upload, and basic record navigation (Next, Previous, Search) functionalities.

---

## Features

- User registration form with the following fields:
  - Name
  - Surname
  - Phone Number
  - Email Address
  - Password
  - Gender
  - Health Problems
  - Health Problem Description

- Upload and crop a profile photo.
- Store all user information and photo in binary files (`index.bin` and `veri.bin`).
- Search, navigate to the next and previous records.
- Display user registration status in a multi-line edit box (MLE).

---

## Technologies Used

- C++ Language
- [ICBYTES Library](https://github.com/cembaykal/ICBYTES)
- `ic_media.h` for image processing
- `icb_gui.h` for GUI operations

---

## How to Run

1. **Clone the Repository**

```bash
   git clone https://github.com/your-username/user-registration-page.git
   cd user-registration-page
```

2. **Set Up the Environment**
- Ensure you have the `ICBYTES` library and its header files (`icbytes.h`, `icb_gui.h`, `ic_media.h`) properly included.
- Use a Windows environment for compatibility.

3. **Compile and Run**

Use your preferred C++ compiler (e.g., Visual Studio) and make sure the project links the required headers and libraries.

---

## Project Structure

- `main.cpp` : Contains the main logic for GUI initialization, user input handling, photo upload, file writing, and record navigation.
- `index.bin` : Stores indexing information (keys and addresses).
- `veri.bin` : Stores actual user data and profile pictures.
- `gym2.bmp` : Background image used in the GUI.

---

## Notes

- User information is concatenated and written as a single ICBYTES block using `WriteICBYTES`.
- Data is read back with `ReadICBYTES` and displayed inside the MLE.
- Special care was taken to ensure safe character appending using a helper `AppendText()` function.
- Uploaded profile photos are resized and cropped to fit 150x150 px frames.

---

## Screenshots
> ![Ekran görüntüsü 2025-04-24 232949](https://github.com/user-attachments/assets/897e47ae-4e8a-452a-bac0-6d7637065363)

> ![Ekran görüntüsü 2025-04-24 232749](https://github.com/user-attachments/assets/42cc557f-159c-4a6f-9b8c-475e587a3b34)

> ![Ekran görüntüsü 2025-04-24 232909](https://github.com/user-attachments/assets/b4d3bfdb-a8c5-4545-8bf9-09c5de6451b3)

---

## License

This project is for educational purposes only.
