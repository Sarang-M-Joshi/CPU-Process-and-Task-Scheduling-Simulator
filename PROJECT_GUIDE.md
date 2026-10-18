# DOOMSDAY // TEMPORAL CORE — Project Guide

This guide describes the project as it is implemented in this workspace. The README contains both design notes and later completion notes; where those disagree, this guide follows the current source and menu. Anything inferred from code organization rather than stated explicitly is marked **Inference**.

## 1. What the project does

DOOMSDAY // TEMPORAL CORE is a C++17 terminal application for exploring CPU process scheduling. You define processes, choose FCFS, Round Robin, or non-preemptive Priority Scheduling, and inspect the simulated execution timeline, events, process states, and performance metrics. A separate DSA Training Chamber lets you exercise common structures and algorithms. The terminal theme is presentation: it does not change scheduling decisions.

The project exists to make scheduling decisions visible and repeatable, and to provide hands-on practice with data structures, algorithms, file handling, modular C++ and assertion-based checks.

## 2. Glossary

| Term | Meaning and tiny example |
|---|---|
| Process | A modeled unit of work asking for CPU time. Example: P1 needs 5 time units. |
| PID (Process ID) | The process's integer identifier; it must be unique in the workload. Example: P1 and P2 cannot both have PID 1. |
| Arrival Time (AT) | Simulated time when the process becomes available. It cannot be negative. Example: AT=3 means P2 joins at time 3. |
| Burst Time (BT) | Total CPU time the process needs; it must be positive. Example: BT=4 means four units of CPU execution. |
| Priority | A numeric rank where a smaller value means higher priority. Example: priority 1 is selected ahead of priority 3 when both are ready. |
| New / arrived | A process that has not arrived yet is New; at its AT it becomes eligible for the Ready state. Example: at time 0, a process with AT=2 is New. |
| Ready queue / Ready | Ready processes are waiting for CPU. The ready queue is their ordered waiting line. Example: `[P2, P3]` means P2 is next, then P3. |
| Running | The process currently using the simulated CPU. Only one process runs at a time. Example: P1 runs from 0 to 2. |
| Completed | The process has used all of its burst and will not run again. Example: P1 with BT=2 completes at time 2 if it starts at 0. |
| CPU idle time / timeline gap | Time while no arrived process is ready, although work remains. The scheduler records an IDLE segment. Example: first arrival at 4 creates an idle interval 0–4. |
| Preemptive scheduling | A running process may be paused before its burst is finished. Round Robin does this when its quantum expires. Example: P1 runs 0–2, then waits for another turn. |
| Non-preemptive scheduling | Once dispatched, a process runs until completion. FCFS and this project's Priority policy are non-preemptive. Example: P1 continues through time 5 even if a higher-priority process arrives at time 1. |
| Time quantum / time slice | Round Robin's maximum run length before a still-unfinished process goes to the queue's rear. Example: quantum 2 gives P1 at most two units per turn. |
| Gantt chart | A time-ordered picture of CPU use. Example: `P1 0–2 | P2 2–4 | IDLE 4–5`. Each bar is one execution segment or idle interval. |
| Completion Time (CT) | Simulated time when a process finishes. Example: finishing at t=8 gives CT=8. |
| Turnaround Time (TAT) | Total time from arrival to completion: `TAT = CT - AT`. Example: CT=8, AT=1 gives TAT=7. |
| Waiting Time (WT) | Time spent waiting rather than using the CPU: `WT = TAT - BT`. Example: TAT=7, BT=3 gives WT=4. |
| Average WT / Average TAT | The arithmetic mean of the corresponding per-process values. Example: waiting times 0 and 4 give average WT 2. |
| CPU Utilization | `busy CPU time / total elapsed time × 100%`; elapsed time is measured from 0 to final completion, including idle time. Example: 6 busy units over 8 elapsed units is 75%. |
| Throughput | Completed processes divided by total elapsed time. Example: 2 completions over 8 units is 0.25 processes per time unit. |
| Simulated time / animation delay | Simulated time is the scheduler's integer clock. Animation delay only slows what the terminal displays; it does not advance that clock. Example: a one-second display pause during a P1 segment does not add a simulated second. |
| Event log / replay | The log stores completed runs' events and timeline data. Replay reads and redraws those saved records; it does not call the scheduler again. Example: replay shows a previously saved P1 dispatch at T=0. |

## 3. Menu tour: what you do → what you see

The actual main menu has seven options. The README's Timeline Archives, Multiverse Comparison and System Configuration are menu options 4, 5 and 6; they are not hidden under another menu, though the Scheduling Lab also offers shortcuts to compare and replay.

| Main option | What you do → what you see |
|---|---|
| **[1] Process Registry** | Add, list, search, update or remove the in-memory process workload → see process records or success/validation messages. |
| **[2] Temporal Simulation** | Select a policy, compare, replay, or load a sample workload → see a live timeline and report, a comparison table, or saved replay. |
| **[3] DSA Training Chamber** | Pick a data structure or algorithm → enter values and operations → see results, structure contents or errors. |
| **[4] Timeline Archives** | Replay the latest completed run → see its saved workload, events, Gantt segments and saved metrics, if present. |
| **[5] Multiverse Comparison** | Enter a positive Round Robin quantum → see FCFS, Round Robin and Priority metrics for the current workload side by side. |
| **[6] System Configuration** | Set animation Full/Reduced/Off or toggle ANSI color when supported → see the selected mode and whether color is active. |
| **[0] Shut Down Core** | Exit → see the shutdown message. |

### Process Registry

Each process has PID, arrival, burst and priority. Add asks for all four values. Display prints every record. Search asks for a PID and prints its record. Update asks for the existing PID and then a replacement PID, arrival, burst and priority. Delete asks for the PID. A duplicate PID, negative arrival, or non-positive burst is rejected; invalid updates leave the old record unchanged. Missing PIDs produce a not-found message. This in-memory list is the workload used by scheduling.

### DSA Training Chamber

Each structure is initialized for the lab and retains its values while you remain in the application. These are practice structures, except for the Round Robin queue noted below.

| Choice / group | What you do → what you see; why the structure matters |
|---|---|
| **1. SLL (singly linked list)** | Insert at beginning/end/index; delete at beginning/end/index; search by value; update by index; traverse; reverse; check empty or size → enter integer values and zero-based indices, then see a value/index, updated list, status, or error. Each node links forward to the next; useful for learning linked sequence updates. |
| **2. DLL (doubly linked list)** | Same operations as SLL, plus backward traversal → see values forward or backward. Each node links to both neighbors, enabling movement in either direction. |
| **3. Array Stack / 4. Linked Stack** | Push a value, pop the top, peek, display, check empty/full, or size → see the value/result or stack contents top to bottom. A stack is last-in, first-out. Array stack has fixed capacity 100 by default and can become full; linked stack has no configured capacity. |
| **5. Circular Queue / 6. Linked Queue** | Enqueue, dequeue, inspect front, display, check empty/full, or size → see FIFO results and contents front to rear. The array queue wraps its indices around to reuse freed slots; default capacity is 100. The linked queue has no configured capacity. |
| **7. Search and Sort** | Choose linear search, binary search, bubble sort, insertion sort or quick sort; enter a count and that many integers. Searches also ask for a key. Search reports its zero-based index or not found; sorts display ascending values. Binary search rejects unsorted ascending input because its comparisons depend on sorted order. |
| **8. Hash Table** | Insert, search, delete or display an integer key → see found/not found, duplicate/full rejection, deletion result, or slots. Open addressing with linear probing checks the next slot after a collision. A tombstone marks a deleted slot so a search can continue along an existing probe chain. |
| **9. Sequential File** | Append asks PID, arrival, burst and priority; display lists records; search/delete ask for PID → see records or success/error. The format is `PID,arrival,burst,priority` in `process_records.csv`. Append rejects duplicate PIDs and invalid arrival/burst values. Delete prepares retained rows in a temporary file, backs up the original before installation and attempts rollback if installation fails. Malformed input fails safely without replacing the original. |

List indices are zero-based. Attempting an invalid index, popping/dequeuing an empty structure, or inserting into a full array structure returns an error/result without a successful mutation.

### Temporal Simulation / Scheduling Lab

| Choice | What you do → what you see |
|---|---|
| **1. FCFS** | Run the current workload → non-preemptive execution in arrival order; equal arrival times use PID order. |
| **2. Round Robin** | Enter a positive quantum and select animation mode → see repeated time-slice segments and ready-queue rotation. Processes arriving during a slice are enqueued before the interrupted process is re-enqueued at the slice boundary. |
| **3. Priority Scheduling** | Run the workload → non-preemptive choice of the smallest priority number; ties use arrival time and then PID. A newly arrived higher-priority process does not interrupt a running process. |
| **4. Compare policies** | Enter the Round Robin quantum → get FCFS, RR and Priority metrics on independent runs of the same unchanged workload; the runs are saved to the event log. |
| **5. Replay latest run** | Choose timed or step-controlled playback if animation is enabled → see saved workload, recorded events and saved Gantt/metrics without rescheduling. Step controls: Enter advances/resumes, `+` speeds up, `-` slows down, `q` exits. |
| **6. Load sample workload** | No input → replaces the current in-memory workload with P1(AT=0,BT=5,PR=2), P2(AT=1,BT=3,PR=1), P3(AT=6,BT=2,PR=3). |
| **0. Back** | Return to main menu. |

Before running, the scheduler validates non-negative arrivals, positive bursts, unique PIDs and (for RR) positive quantum. The live view shows simulated time, running/idle segment, ready queue, process-state snapshots, remaining burst, a growing Gantt timeline, and timestamped arrival/dispatch/preemption/completion events. The report shows per-process CT/TAT/WT, their averages, CPU utilization and throughput. Compare all uses copies/results from the same workload so policies do not mutate one another's input.

### Timeline Archives, comparison and configuration

Timeline Archives opens the latest complete entry in `simulation_events.log`. Completed individual scheduling runs and each policy result of a comparison are appended there. The archive currently replays the most recently completed run record; it is not a browser for selecting among arbitrary run IDs. Replay animation can be timed or step controlled. The log stores events, execution segments, workload and metrics in a simple text format.

Multiverse Comparison can be opened directly from the main menu or as option 4 inside Temporal Simulation. It asks for a Round Robin quantum and prints a side-by-side comparison. The comparison path saves all three results to the archive.

System Configuration sets Full, Reduced or Off display animation and toggles ANSI color if the terminal supports it. `DOOMSDAY_ANIMATION=off` and `DOOMSDAY_ANIMATION=reduced` set startup animation behavior; `NO_COLOR` disables color. Replay speed and animation affect display only.

## 4. Internals and module map

### How a schedule is produced

The `Process` model stores the four input fields. The scheduler validates the workload, creates a deterministic arrival ordering, and simulates integer time. FCFS selects the earliest available arrival. Priority selects the best available priority under the tie rules. Both run a chosen process to completion. Round Robin stores process indices in the array-based circular queue, runs the front process for up to the quantum, admits arrivals through the slice end, then re-enqueues an unfinished process. If no process is ready, the scheduler adds an idle segment until the next arrival.

The result contains timeline segments, snapshots of process states and remaining bursts, a ready-queue snapshot, timestamped events, and per-process metrics. The UI renders those results and appends completed runs. Replay parses saved records and renders them; it does not invoke a policy. The DSA demonstration linked lists, stacks, linked queue, search/sort and hash table are not dependencies of the scheduling policies. The array circular queue is used by RR; the other practice structures stay in the DSA Lab.

**Inference from implementation:** the scheduler's FCFS and Priority ready selection scans process records, while Round Robin uses the circular queue to preserve rotation order. Event records are generated from scheduler results; UI code handles saving, reading and rendering them rather than a separate `simulation/` module.

### Why the project uses `.h` files

In C++, `.h` conventionally means a header file. Headers declare types and functions other files need, and often contain small class/function implementations. `#include` makes those declarations visible to a translation unit, while `#pragma once` prevents the same header from being included repeatedly in that compilation. This project keeps most modules header-only: for example, `Process`, DSA structures, storage and UI are defined in headers. The scheduler separates declarations in `Scheduler.h` from implementation in `Scheduler.cpp`. This lets `main.cpp`, tests and other modules share consistent interfaces without duplicating declarations. The compiler still compiles included code into the executable; headers are not separate runtime components.

### Folder and file guide

| Path | Purpose |
|---|---|
| `main.cpp` | Program entry point; constructs/runs the menu and handles top-level exceptions. |
| `models/Process.h` | Defines `scheduler::Process` and its simple display helper. |
| `scheduler/Scheduler.h` | Scheduling policies, process states, snapshots, run segments, events, metrics and result types. |
| `scheduler/Scheduler.cpp` | Validation, FCFS/RR/Priority simulation, tie rules, metrics and event generation. |
| `dsa/Maybe.h` | Optional-style return type for operations that may have no value, such as pop or search miss. |
| `dsa/SLL.h`, `dsa/DLL.h` | Singly and doubly linked integer list implementations. |
| `dsa/StackArray.h`, `dsa/StackLinked.h` | Fixed-capacity array and linked-node stack implementations. |
| `dsa/QueueArray.h`, `dsa/QueueLinked.h` | Fixed-capacity circular array and linked-node queues. `QueueArray` is used by RR. |
| `dsa/SearchSort.h` | Linear/binary search and bubble/insertion/quick sort helpers. |
| `dsa/HashTable.h` | Integer hash table using open addressing and linear probing with deletion tombstones. |
| `storage/SequentialProcessFile.h` | Sequential CSV process record read, append, find and guarded rewrite/delete operations. |
| `ui/Menu.h` | Menus and interaction flows for registry, labs, scheduling, comparison, replay and configuration; renders scheduler results. |
| `ui/Terminal.h` | Terminal color detection, animation modes, and display delays/reveal helpers. |
| `tests/structures_tests.cpp` | Assertion-based checks across DSA structures, storage, scheduler metrics, ordering and error cases. |
| `demo.ps1` | Windows PowerShell demonstration/verification script; details below. |
| `process_records.csv` | Sequential-file DSA lab data file. It is separate from the process registry's in-memory workload. |
| `simulation_events.log` | Appended saved run/event/timeline/metrics data used for latest-run replay. |
| `cpu_scheduler.exe` | Built Windows executable currently present in the workspace; generated artifact, not source. |
| `run.txt` | Small workspace/run artifact. **Inference:** it appears to be an invocation or output note; it is not referenced by the C++ source. |

There is no `simulation/` directory in the actual tree. README's proposed layout lists `storage/` twice and includes planned `simulation/` and `data/` directories that are not separate implementation folders here.

## 5. Worked example

Workload:

| PID | AT | BT | Priority |
|---|---:|---:|---:|
| P1 | 0 | 5 | 2 |
| P2 | 1 | 3 | 1 |

### Round Robin, quantum 2

1. At time 0 only P1 has arrived, so it runs for 2 units: **P1 0–2**. One unit remains for P1. P2 arrived during this slice and is queued before P1 is put back.
2. P2 is at the front, so it runs for 2 units: **P2 2–4**. One unit remains for P2; P1 is waiting behind it.
3. P1 runs for 2 units: **P1 4–6**. It finishes at time 6.
4. P2 runs its last unit: **P2 6–7**. It finishes at time 7.
5. P1 runs its final unit: **P1 7–8**. It finishes at time 8.

Timeline: `P1 0–2 | P2 2–4 | P1 4–6 | P2 6–7 | P1 7–8`.

| PID | CT | TAT = CT−AT | WT = TAT−BT |
|---|---:|---:|---:|
| P1 | 8 | 8 | 3 |
| P2 | 7 | 6 | 3 |

Average WT = **3**; average TAT = **7**. Total busy and elapsed time are both 8, so CPU utilization is **100%**. Throughput is 2/8 = **0.25 processes per time unit**.

### FCFS and Priority on the same workload

At time 0 only P1 is ready. Both policies are non-preemptive, so P1 runs until time 5 even though P2 arrives at time 1. P2 then runs from 5 to 8. For Priority, P2's higher priority cannot interrupt P1 after dispatch.

| Policy | Timeline | Average WT | Average TAT |
|---|---|---:|---:|
| FCFS | P1 0–5 | P2 5–8 | 2 | 6 |
| Priority | P1 0–5 | P2 5–8 | 2 | 6 |
| Round Robin, q=2 | P1 0–2 | P2 2–4 | P1 4–6 | P2 6–7 | P1 7–8 | 3 | 7 |

FCFS and Priority happen to match here because the higher-priority P2 is not available when P1 starts, and the policies do not preempt. All runs have 100% utilization and throughput 0.25 for this no-idle workload.

## 6. Testing and verification

`tests/structures_tests.cpp` uses C++ `assert` statements. It checks list, stack and queue operations (including circular wraparound), search/sort, hash collisions/tombstones/full-table cases, sequential file read/write/delete and malformed-file preservation, and scheduling metrics, states, events, idle gaps, tie ordering and invalid quantum/duplicate PID cases.

`demo.ps1` is a Windows PowerShell script that changes to the project directory, compiles the app and assertion executable with C++17 warning flags, runs the checks, drives the sample comparison and timed replay, checks step-controlled replay, and smoke-checks animation-off plain-text startup and a DSA menu operation. It sets/restores environment variables around checks. It also writes generated executables and test CSV artifacts in the project directory; it is a runnable demo/check script, not a scheduler module.

**Verification note:** the README states the checks passed with MinGW GCC 6.3.0 and describes the demo's expected outputs. That is a recorded README claim, not a fresh run performed while writing this guide.

## 7. Edge cases and error handling

- **Empty workload:** the UI says it is showing an empty workload. No per-process metric rows are produced. **Implementation detail:** aggregate values remain initialized to zero when there are no processes, avoiding division by zero.
- **Duplicate PID:** registry add/update and scheduler validation reject duplicates; sequential-file append rejects a PID already in the file.
- **Invalid inputs:** negative arrival and burst `<= 0` are rejected. Round Robin quantum must be `> 0`. Non-integer console input is cleared and prompts again.
- **Idle gaps:** if all arrived work is exhausted but a process remains in the future, an IDLE segment is drawn and contributes to elapsed time. For example, a sole process arriving at 3 with BT=2 yields idle 0–3, then work 3–5.
- **Malformed CSV:** sequential file operations fail safely; delete does not install rewritten data if reading/parsing fails. Append also fails if existing content is malformed.
- **Empty/full DSA structures:** pop/dequeue/search-like operations can return no value; fixed arrays report full rather than overwriting values.
- **Replay input:** no archive or empty/malformed archive produces a message and returns. Older legacy entries without saved metrics can still be replayed with a legacy notice.
- **Capacity/range:** Round Robin rejects a workload too large for its integer-index circular queue. The scheduler also validates supported time arithmetic range before simulation.

## 8. Limitations and README inconsistencies

- This is a discrete simulation, not a real operating-system scheduler. It models CPU time and process state; it does not launch OS processes or measure hardware performance.
- The Process Registry is in memory and resets on program restart. `process_records.csv` is managed separately through the DSA sequential-file submenu; the source does not automatically load it into the registry.
- Timeline Archives replays the most recent completed run parsed from the append-only log; the UI does not offer a full archive browser or run-ID selector.
- The archive log is a lightweight text format rather than a database. Replay timing is a display effect and does not reconstruct execution decisions.
- The README's older Day 1 statement says scheduling is planned and not implemented. That is stale: the current tree has FCFS, RR and Priority implementations, and the later README completion section recognizes them.
- The README's architecture tree labels itself proposed, includes a not-present `simulation/` and `data/` folder, and lists `storage/` twice. The actual event/replay functionality lives mainly in `ui/Menu.h`, with event/result types and generation in `scheduler/`.
- The README has older checklist boxes still unchecked beside later sections saying those same features and tests are complete. Treat the later completion section plus current code as the status, while recognizing the unchecked checklist was not updated.
- Priority is explicitly non-preemptive in the implemented code. Descriptions that imply an arriving higher priority process interrupts a running task would be inaccurate.
- **Inference/ambiguity:** the menu title “Process Registry” may sound persistent, but its actual `processes_` collection is in memory; persistence is only exposed as a separate sequential-file DSA exercise.
