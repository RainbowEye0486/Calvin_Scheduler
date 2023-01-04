import json
import random

dataNums = 1361
writeFileName = "KizilayDataset/KData"
PRINT = 0

work_ratio = [0.3, 0.5, 0.8, 1]


# Class that record details in the task
class Task:
    def __init__(self, num):
        self.num = num
        self.time = 0
        self.and_pair = []
        self.or_pair = []


# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    random.seed()
    for i in range(1, dataNums):
        # for each assembly task in dataset
        task_num = 0
        station = 0
        cycle = 0
        task_list = []
        setup_list = []
        with open(r"rawData/DData" + str(i) + ".dat") as readFile:
            print("generate data: " + str(i))
            with open(writeFileName + str(i) + ".json", 'w') as writeFile:
                # print("[system]: write dataFile open success")
                default_init = {
                    "taskNum": 0,
                    "station": 0,
                    "cycleTime": 0,
                    "task": [],
                    "setup": []
                }
                json_object = json.dumps(default_init)
                writeFile.write(json_object)
            writeFile = open(writeFileName + str(i) + ".json", 'r+')
            wData = json.load(writeFile)
            setup_cnt = 0
            for rdata in readFile:
                tmp_setup = []
                code = rdata.split(';')[0].split(' ')[0]
                if code == 'T':
                    # print("task: " + rdata.split(';')[0].split(' ')[2])
                    task_num = int(rdata.split(';')[0].split(' ')[2])
                    wData["taskNum"] = task_num
                    for tn in range(1, task_num + 1):
                        task_list.append(Task(tn))
                elif code == 'S':
                    # print("station: " + rdata.split(';')[0].split(' ')[2])
                    wData["station"] = int(rdata.split(';')[0].split(' ')[2])
                elif code == 'C':
                    # print("cycle time: " + rdata.split(';')[0].split(' ')[2])
                    wData["cycleTime"] = int(rdata.split(';')[0].split(' ')[2])
                elif code == 't':
                    # print("task spend: " + rdata.split(';')[0].split(' ', 2)[2])
                    task_spend = rdata.split(';')[0].split(' ', 2)[2].strip('[').strip(']').split(' ')
                    for ts in range(0, task_num):
                        task_list[ts].time = int(task_spend[ts])
                elif code == 'P':
                    # print("AND priority: " + rdata.split(';')[0].split(' ', 2)[2])
                    and_prio = rdata.split(';')[0].split(' ', 2)[2].strip('{').strip('}').strip(' ').replace('<', ''). \
                        replace('>', '').split(' ')
                    for ap in and_prio:
                        if ap == '':
                            break
                        _ap = ap.split(',')
                        task_list[int(_ap[0]) - 1].and_pair.append(int(_ap[1]))
                elif code == "POR":
                    # print("OR priority: " + rdata.split(';')[0].split(' ', 2)[2])
                    or_prio = rdata.split(';')[0].split(' ', 2)[2].strip('{').strip('}').strip(' ').replace('<', ''). \
                        replace('>', '').split(' ')
                    for op in or_prio:
                        if op == '':
                            break
                        _op = op.split(',')
                        task_list[int(_op[0]) - 1].or_pair.append(int(_op[1]))
                elif code == "setup":
                    # print(rdata.split(' ', 2)[2].strip('[').split(' '))
                    for sp in range(0, task_num):
                        tmp_setup.append(int(rdata.split(' ', 2)[2].strip('[').split(' ')[sp]))
                    setup_list.append(tmp_setup)
                    setup_cnt += 1
                else:
                    # print(rdata.strip('[').split(' '))
                    for sp in range(0, task_num):
                        tmp_setup.append(int(rdata.strip('[').split(' ')[sp]))
                    setup_list.append(tmp_setup)
                    setup_cnt += 1
                    if setup_cnt == task_num:
                        break
            json_tasks = []
            wData["setup"] = setup_list
            for tn in range(0, task_num):
                # decided who should perform the job
                rand = random.random()
                machine = ''
                if rand <= work_ratio[0]:
                    machine = 'h'
                elif work_ratio[0] < rand <= work_ratio[1]:
                    machine = 'r'
                elif work_ratio[1] < rand <= work_ratio[2]:
                    machine = 'b'
                else:
                    machine = 'e'
                json_task = {
                    "number": task_list[tn].num,
                    "time": task_list[tn].time,
                    "and": task_list[tn].and_pair,
                    "or": task_list[tn].or_pair
                }
                # not assign any machine yet
                json_tasks.append(json_task)
            wData["task"] = json_tasks
            writeFile.seek(0)
            writeFile.write(json.dumps(wData))
    print("Dataset generate finished!")
