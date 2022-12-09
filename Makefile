all: mem_report task_report

task_report: task_report.c
    gcc -Wall -fsanitize=address -o task_report task_report.c
    
mem_report: mem_report.c
    gcc -Wall -fsanitize=address -o mem_report mem_report.c
    
clean:
    rm task_report mem_report