""" 
    R Detobel - N Liccard - D Hoornaert
    Computer Sciences - Master ULB 
    ELEC-H-473
    Fev 2018
    ---------------------
    Auto convert address
"""
import sys
import requests
import webbrowser
import time

TEST_URL_ADD = 'http://164.15.75.7/'

def convertLabel(sourceFile, destFile):
    dictLabel = {}

    readFile = open(sourceFile, 'r')

    contenu = readFile.read()

    counter = 0
    for line in contenu.split("\n"):
        line = line.split("//")[0]
        line = line.strip()

        label = ""
        labelLine = line.split(":")
        if(len(labelLine) > 2):
            print("Multiple label Oo ! (line: " + str(counter) + " '" + line + "')")
            return None

        elif(len(labelLine) == 2):
            label = labelLine[0].strip()
            line = labelLine[1].strip()
            dictLabel[label] = counter

        if(line != ""):
            counter += 1

        # Double if movi
        splitLine = line.split(" ")
        if(splitLine[0].lower() == "movi"):
            counter += 1

    newFile = open(destFile, 'w')

    for label in dictLabel:
        contenu = contenu.replace("<? " + label + " ?>", "0x{:04x}".format(dictLabel[label]) + " // Addr: " + label)

    newFile.write(contenu)

def testFile(sourceFile, exerciceName):
    global TEST_URL_ADD

    files = {'file': open(sourceFile, 'rb')}
    data = {'exo': exerciceName, 'archi': 'IS0', 'logic': '0', 'asm_digest': '1', 'exec': '100000'}

    r = requests.post(TEST_URL_ADD, files=files, data=data)

    print("Work in progress...")
    webbrowser.open_new_tab(r.url)


if len(sys.argv) <= 2:
    if(len(sys.argv) > 1 and (sys.argv[1] in ("help", "-h", "--help"))):
        print("Execute with: python " + sys.argv[0] + " <file to read> <file to produce> <option>")
        print("The program will replace '<? label ?>' from '<file to read>' to hex address to a file: '<file to produce>'")
        print("Options:")
        print("\t-t/--test [exercice]\tTest file online")
        print("\t\t\t\tWhere [exercice] is the name of the exercice on the website (e.i. '1.5_b15_extract.txt')")

    else:
        print("[ERROR] Missing argument: python " + sys.argv[0] + " <file to read> <file to produce> <option>")
        print("Execute: 'python " + sys.argv[0] + "help' for more infos")

else:
    convertLabel(sys.argv[1], sys.argv[2])
    if(len(sys.argv) > 3):
        if(sys.argv[3] in ("-t", "--test")):
            if(len(sys.argv) >= 4):
                testFile(sys.argv[2], sys.argv[4])
            else:
                print("[ERROR] Please precise the exercice name")
        else:
            print("[ERROR] Unknown paramter: " + sys.argv[3])


