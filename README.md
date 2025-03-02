# Interactive Teapot (OpenGL + GLFW)

This is an **interactive teapot** implemented using **OpenGL** and **GLFW**. The project is built as a **Visual Studio** project and allows you to explore a 3D teapot model with basic movement and zoom controls.

## 📦 Building the Project

To generate the `.exe` file, follow these steps:

1. Open the project in **Visual Studio**.
2. Press **`Ctrl + Shift + B`** to build the project.
3. The executable file will be generated automatically.

## ▶ Running the Executable

After building, you can run the executable by providing the required `.obj` file as an argument.

```sh
./your_executable assets/teapot.obj
```

## 🎮 Controls

- **Arrow Keys** → Move in the plane you are facing.
- **Alt + Right Mouse Drag** → Zoom in and out.

## 📁 Project Structure
```
📂 project-root/
├── 📂 assets/
│   ├── teapot.obj  # 3D model file
├── 📂 src/
│   ├── main.cpp    # Main source code
│   ├── shader.cpp  # Shader handling
├── 📂 include/
│   ├── glfw/       # GLFW library files
│   ├── glm/        # GLM math library
├── 📂 build/       # Build output
└── README.md       # This file
```

## 🔧 Dependencies

Ensure you have the following installed before building:
- **OpenGL**
- **GLFW**
- **GLM (OpenGL Mathematics Library)**

## 🚀 Contribution
Feel free to fork this repository and submit pull requests for improvements!

---

Made with ❤️ using OpenGL & GLFW.
