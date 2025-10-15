# GitHub Copilot Instructions

## 1. Coding Style

- Support **C / C++ / Python** source files.  
- Follow the **Google C++ / Python Style Guide** as a base.  
- For C code, maintain clear structure and consistent naming conventions.  
- Use **4 spaces** for indentation.  
- Use **camelCase** for variables, **snake_case** for functions, and **PascalCase** for classes.  
- Always include **clear inline comments** for non-obvious logic.  
- Keep functions **under 200 lines** and ensure **one clear purpose per function**.

---

## 2. Project Rules

- Do **not** suggest unnecessary or heavy libraries.  
- Keep functions short, modular, and focused. (≤ 200 lines recommended)  
- Every function **must include a Doxygen-compatible comment block**, such as:  
  ```c
  /**
   * @brief   Reads and calibrates sensor data.
   * @param   raw_data  Raw ADC input value.
   * @return  Calibrated sensor value (float).
   */
  ```

- Use Doxygen tags such as @brief, @param, @return, @note, and @warning.
- Prefer safe and modern APIs over deprecated ones.
- Complex functions may include Flow Charts using PlantUML or Graphviz syntax:
  ```c
    /**
     * @startuml
     * start
     * :Init Sensor;
     * :Read Data;
     * :Filter;
     * :Return Result;
     * stop
     * @enduml
    */
  ```
  or equivalently:
  ```c
   /**
    * @dot
    * digraph flow {
    *   start -> init -> read -> filter -> output;
    * }
    * @enddot
    */
  ```
## 3. Documentation & Comments

- All comments must be written in English and compatible with Doxygen syntax.
- Comment every function, loop, and conditional block that contains critical logic.
- Use TODO comments to highlight areas needing future work or review.
- Use TODO format as:
```
TODO(username): description
```

- When appropriate, include text-based Flow Charts inside comment blocks to visualize logic.
- The project’s Doxygen configuration should enable:

```
HAVE_DOT = YES
CALL_GRAPH = YES
CALLER_GRAPH = YES
```
→ This allows automatic Call Flow graphs during documentation build.

## 4. Copilot Behavior

- Suggest code consistent with the project’s existing structure.
- Avoid generating placeholder variables like foo or bar.
- Recognize comments starting with # request: or // request: as natural-language commands.
- Infer logical patterns from context instead of duplicating code.
- When generating comments, prefer Doxygen-compatible syntax.
- If flowchart-like explanations are needed, use PlantUML (@startuml) style by default.

## 5. Copilot Chat (Optional)

- When Copilot Chat is active:
  /explain → Summarize the current file or selection in clear English.
  /tests → Generate minimal unit tests only for newly modified functions.
  /fix → Suggest safe modifications without deleting unrelated code.
  /diagram → Provide textual or UML-style Flow Chart of logic.
  /callflow → Analyze and summarize the function Call Flow within the current file.
  Example output:
  ```
  Call Flow:
  main() → system_init() → sensor_read() → process_data() → send_uart()
  ```