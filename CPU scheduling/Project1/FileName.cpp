#include <iostream>
#include <fstream>
#include <iomanip>
#include <queue>
using namespace std;
struct Process
{
        int id;
        int arrival_time; 
        int burst_time;
        int remaining_time;
        int completion_time;
        int waiting_time;
        int turnaround_time;
        int start_time;
};

void calculateTimes(vector<Process>& processes) {
    for (auto& p : processes) {
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
    }
    cout << "\nProcess Results:\n";
    for (auto& p : processes) {
            cout << "P" << p.id
            << ": Finish Time=" << p.completion_time
            << ", Waiting Time=" << p.waiting_time
            << ", Turnaround Time=" << p.turnaround_time << "\n";
    }
}

void calculateCPUUtilization(vector<Process>& processes) {
    int total_busy_time = 0, total_time = 0;
    int min_start_time = 0, max_completion_time = 0;

    for (auto& p : processes) {
        total_busy_time += p.burst_time;
        min_start_time = min(min_start_time, p.start_time);
        max_completion_time = max(max_completion_time, p.completion_time);
    }
    total_time = max_completion_time - min_start_time;

    double cpu_utilization = (double) (total_busy_time / total_time) * 100;
    cout << "\nCPU Utilization: " << fixed << setprecision(2) << cpu_utilization << "%\n";
}

void displayGanttChart(vector<pair<int, int>>& timeline) {
    cout << "\nGantt Chart:\n";
    cout << "--------------------------------------------------\n|";
    for (auto& t : timeline) {
        cout << " P" << t.first << " |";
    }
    cout << "\n--------------------------------------------------\n0";
     for (auto& t : timeline) {
        cout << setw(5) << t.second;
    }
    cout << "\n";
}

void runFCFS(vector<Process> processes) {
    cout << "\n             =======  FCFS  =======\n";
    int time = 0;
    vector<pair<int, int> > timeline;
        for (auto& p : processes) {
        if (time < p.arrival_time) 
            time = p.arrival_time;
        p.start_time = time;
        time += p.burst_time;
        p.completion_time = time;
        timeline.push_back({ p.id, time });
    }

    displayGanttChart(timeline);
    calculateTimes(processes);
    calculateCPUUtilization(processes);
}

void runRR(vector<Process> processes, int quantum) {
    cout << "\n             ======  RR  ======\n";

    int time = 0;
    queue<Process> q;
    vector<pair<int, int>> timeline;
    for (auto& p : processes) 
    p.remaining_time = p.burst_time;
    
    int idx = 0;
    q.push(processes[idx++]);
        while (!q.empty()) {
        Process current = q.front();
        q.pop();

        if (current.remaining_time == current.burst_time) 
            current.start_time = max(time, current.arrival_time);
        int exec_time = min(quantum, current.remaining_time);
        time = max(time, current.arrival_time) + exec_time;
        current.remaining_time = current.remaining_time - exec_time;

        timeline.push_back({ current.id, time });

        if (current.remaining_time > 0)
            q.push(current);
        else
            current.completion_time = time;

        while (idx < processes.size() && processes[idx].arrival_time <= time)
        q.push(processes[idx++]);

        processes[current.id - 1] = current;
    }

    displayGanttChart(timeline);
    calculateTimes(processes);
    calculateCPUUtilization(processes);
}

int main() {
    vector<Process> processes;
    int n, quantum;

    ifstream input("input.txt");
    if (!input) {
        cout << "Error: Cannot open input file!\n";
        return 1;
    }
  input >> n;
    for (int i = 0; i < n; i++) {
        Process p;
        input >> p.id >> p.arrival_time >> p.burst_time;
        processes.push_back(p);
    }
    input >> quantum;
    input.close();
 
    runFCFS(processes);
    runRR(processes, quantum);
    return 0;
}
