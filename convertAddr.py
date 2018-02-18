""" 
    R Detobel - N Liccard - D Hoornaert
    Computer Sciences - Master ULB 
    ELEC-H-473
    Fev 2018
    ---------------------
    Auto convert address
"""
import sys

dictLabel = {}

if len(sys.argv) <= 2:
    if(len(sys.argv) > 1 and (sys.argv[1] == "help" or sys.argv[1] == "-h" or sys.argv[1] == "--help")):
        print("Execute with: python " + sys.argv[0] + " <file to read> <file to produce>")
        print("The program will replace '<? label ?>' from '<file to read>' to hex address to a file: '<file to produce>'")

    else:
        print("Error of argument: python " + sys.argv[0] + " <file to read> <file to produce>")

else:
    readFile = open(sys.argv[1], 'r')

    contenu = readFile.read()

    counter = 0
    for line in contenu.split("\n"):
        line = line.split("//")[0]
        line = line.strip()

        label = ""
        labelLine = line.split(":")
        if(len(labelLine) > 2):
            print("Multiple label Oo ! (line: " + str(counter) + " '" + line + "')")
            sys.exit()

        elif(len(labelLine) == 2):
            label = labelLine[0].strip()
            line = labelLine[1].strip()
            dictLabel[label] = counter

        if(line != ""):
            counter += 1

        # Double if movi
        splitLine = line.split(" ")
        if(splitLine[0] == "movi"):
            counter += 1

    newFile = open(sys.argv[2], 'w')

    for label in dictLabel:
        contenu = contenu.replace("<? " + label + " ?>", "0x{:04x}".format(dictLabel[label]))

    newFile.write(contenu)
