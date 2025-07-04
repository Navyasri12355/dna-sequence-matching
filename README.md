# 🧬 DNA Sequence Matching Platform

A full-stack application that allows users to analyze DNA sequences using six classical string matching algorithms. This project is built using a **C-based backend**, **Flask API**, and a **JavaScript-powered web frontend**. It is designed for educational use, bioinformatics experimentation, and algorithm performance comparison.

---

## 🔍 Features

- Pattern matching using six algorithms:
  - **Naive**
  - **Knuth-Morris-Pratt (KMP)**
  - **Rabin-Karp**
  - **Boyer-Moore**
  - **Aho-Corasick**
  - **Finite Automata**
- Match index highlighting and match count
- Execution time (ms) shown per algorithm
- Best, Worst, and Average Time Complexities displayed
- Compare-All mode to show which algorithm is the fastest for the current input

---

## 🧠 System Architecture

The system uses a hybrid backend-frontend model:

- **C Backend**: All core algorithms are written in C for speed and compiled into a Python `.pyd` (shared object) module.
- **Flask API**: Acts as middleware to receive inputs, call C functions, and return structured results.
- **Frontend**: Built with HTML, CSS, and JavaScript to take user inputs and visualize the results interactively.

---

## 💻 Tech Stack

| Layer      | Technology        |
|------------|-------------------|
| Algorithms | C (compiled as `.pyd`) |
| API        | Python 3.12 + Flask |
| Frontend   | HTML, CSS, JavaScript |

---

## 🚀 How to Run Locally

### 1. Clone the Repository

```bash
git clone https://github.com/Navyasri12355/dna-sequence-matching.git
cd dna-sequence-matching
```

### 2. Setup Python Environment
Install Flask:
```bash
pip install flask flask_cors
```

### 3. Compile the C Code to .pyd
Ensure you're using Python 3.12+ and GCC/MinGW on Windows:
```bash
python setup.py build_ext --inplace
```

### 4. Run Flask Server
```bash
python app.py
```

### 5. Launch Frontend
Open frontend/index.html in any browser to start using the platform.
