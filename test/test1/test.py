import subprocess
import os

EXAMPLES_DIR = 'test/test1/examples'
PROJECT_ROOT = ''

def main():

    for exampleDirName in os.listdir(EXAMPLES_DIR):

        if not (os.path.exists(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.san')) and 
                os.path.exists(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.in')) and
                os.path.exists(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.out'))):

            continue

        # run Yay
        with open(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.san')) as configFile:
            configFileContent = configFile.read()
            
            generateP = subprocess.run([os.path.join(PROJECT_ROOT, 'build/src/Yay')], input=configFileContent, capture_output=True, text=True)

            print(generateP.stdout)

        # run yay and compare output

        with open(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.in')) as inputFile, open(os.path.join(EXAMPLES_DIR, exampleDirName, 'test.out')) as outputFile:
            inputFileContent = inputFile.read()
            outputFileContent = outputFile.read()


            YayProcess = subprocess.run([os.path.join(PROJECT_ROOT, 'build/src/analizator/Main')], input=inputFileContent, capture_output=True, text=True)


            if YayProcess.stdout == outputFileContent:
                print('Test passed for', exampleDirName)
            else:
                print('Test failed for', exampleDirName)
                print(YayProcess.stderr)
                print(YayProcess.stdout, '\n\n', outputFileContent)
                break
        

if __name__ == '__main__':
    main()