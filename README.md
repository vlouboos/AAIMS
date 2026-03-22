# AAIMS - Academic Affairs Information Management System
[English](./README.md) | [简体中文](./README_CN.md)

---

## Project Overview
AAIMS is a robust, high-performance C++ solution designed for academic administration. It provides a structured framework for managing student records, complex course hierarchies, and multi-dimensional academic performance data.

## Features

### 1. Administrative Data & Records Management
* **Comprehensive Student Profiling**: Efficient management of core identity data, including Student ID, Name, Gender, Age, Residential/Dormitory details, and Contact Information.
* **Advanced Course Architecture**:
    * Manage Course IDs, Titles, Schedules, and Credit allocations.
    * **Multi-Factor Grading**: Supports a composite scoring model (Classwork, Mid-term/Final Exams, and Final Weighted Grades).
    * **Persistent Retake Tracking**: Specialized logic for course retakes, including semester-specific history and multiple attempt tracking.

### 2. Enrollment & Academic Scheduling
* **Digital Course Registration**: Streamlined enrollment interface for students based on course IDs.
* **Personalized Timetables**: Real-time generation of individual weekly class schedules.

### 3. Performance Assessment
* **Grade Processing**: Dedicated faculty interface for batch grade entry and performance updates.
* **Academic Transparency**: Instant access for students to track subject-specific grades and cumulative credit status.

### 4. Curriculum Analytics & Reporting
* **Class-Centric Analytics**: Aggregates enrollment metrics and examination pass rates by administrative class.
* **Course-Centric Analytics**: Generates student rosters and grade distribution reports for specific subjects.
* **Academic Progress Audit**: Automated calculation of earned credits and identification of failed courses.

### 5. Architecture
* **High-Precision Querying**: Rapid retrieval of complete student dossiers via unique identifiers.
* **Multi-Terminal Connectivity**: Engineered for distributed environments, supporting concurrent data access.

---

## Build Instructions

### Prerequisites
* **Compiler**: A C++ compiler supporting **C++23** (GCC 13+, Clang 15+, or MSVC 19.33+).
* **Build System**: [CMake](https://cmake.org/) (Version 3.20+ recommended for C++23 features).
* **Dependencies**: [nlohmann/json](https://github.com/nlohmann/json) [uNetworking/uWebSockets](https://github.com/uNetworking/uWebSockets) [madler/zlib](https://github.com/madler/zlib) [libuv/libuv](https://github.com/libuv/libuv) (Fetched automatically).

### Compiling from Source
```bash
git clone https://github.com/vlouboos/AAIMS.git
cd AAIMS
cmake -B build -DCMAKE_CXX_STANDARD=23
cmake --build build
```

## License
This project is protected by [BSD-3 Clause License](./LICENSE "点击查看").

---
© 2026 vlouboos. All rights reserved.
