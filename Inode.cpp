
#include <iostream>

#include <vector>

#include <map>

#include <assert.h>

#include <string.h>

#include <math.h>

#include <sys/types.h>

#include <unistd.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <string>

using namespace std;

#define DISK_SIZE 512

// Function to convert decimal to binary char
char decToBinary(int n) {
    return static_cast < char > (n);
}
// #define SYS_CALL
// ============================================================================
class fsInode {
    int fileSize;
    int block_in_use;
    int directBlock1;
    int directBlock2;
    int directBlock3;
    int singleInDirect;
    int doubleInDirect;
    int block_size;
    int freeFilePlace;
    int numOfIndexBlocks;
public:
    fsInode(int _block_size) {
        fileSize = 0;
        block_in_use = 0;
        block_size = _block_size;
        directBlock1 = -1;
        directBlock2 = -1;
        directBlock3 = -1;
        singleInDirect = -1;
        doubleInDirect = -1;
        freeFilePlace = ((block_size * block_size) + (block_size) + (3)) * block_size;
        numOfIndexBlocks = 0;
    }
    // get and set functions for all the attributes of class fsInode:
    int getFreeFilePlace() {
        return this -> freeFilePlace;
    }
    void setFreeFilePlace(int newFree) {
        this -> freeFilePlace = newFree;
    }
    int getFileSize() {
        return this -> fileSize;
    }
    void setFileSize(int size) {
        this -> fileSize = size;
    }
    int getBlockInUse() {
        return this -> block_in_use;
    }
    void setBlockInUse(int blocks) {
        this -> block_in_use = blocks;
    }
    int getDirectBlock1() {
        return this -> directBlock1;
    }
    void setDirectBlock1(int direct) {
        this -> directBlock1 = direct;
    }
    int getDirectBlock2() {
        return this -> directBlock2;
    }
    void setDirectBlock2(int direct) {
        this -> directBlock2 = direct;
    }
    int getDirectBlock3() {
        return this -> directBlock3;
    }
    void setDirectBlock3(int direct) {
        this -> directBlock3 = direct;
    }
    int getSingleInDirect() {
        return this -> singleInDirect;
    }
    void setSingleInDirect(int singleNum) {
        this -> singleInDirect = singleNum;
    }
    int getDoubleInDirect() {
        return this -> doubleInDirect;
    }
    void setDoubleInDirect(int doubleNum) {
        this -> doubleInDirect = doubleNum;
    }
    int getNumOfIndexBlocks() {
        return this -> numOfIndexBlocks;
    }
    void setNumOfIndexBlocks(int newNum) {
        this -> numOfIndexBlocks = newNum;
    }
};

// ============================================================================
class FileDescriptor {
    pair < string, fsInode * > file;
    bool inUse;

public:
    FileDescriptor(string FileName, fsInode * fsi) {
        file.first = FileName;
        file.second = fsi;
        inUse = true;
    }

    string getFileName() {
        return file.first;
    }
    void setFileName(string newName) {
        this -> file.first = newName;
    }
    fsInode * getInode() {

        return file.second;

    }
    int GetFileSize() {
        return file.second -> getFileSize();
    }
    bool isInUse() {
        return (inUse);
    }
    void setInUse(bool _inUse) {
        inUse = _inUse;
    }
};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================
class fsDisk {
    FILE * sim_disk_fd;
    bool is_formated;
    int freeBlocksNum;
    int numOfFormat;
    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied).
    int BitVectorSize;
    int * BitVector;

    // Unix directories are lists of association structures,
    // each of which contains one filename and one inode number.
    map < string, fsInode * > MainDir;
    vector < FileDescriptor > deletedInodes;
    // OpenFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    vector < FileDescriptor > OpenFileDescriptors;
    int block_size;

public:
    // ------------------------------------------------------------------------
    fsDisk() {
        // initialize the attributes to default values:
        numOfFormat = 0;
        block_size = 0;
        freeBlocksNum = 0;
        BitVectorSize = 0;
        BitVector = nullptr;
        is_formated = false;
        sim_disk_fd = fopen(DISK_SIM_FILE, "r+");
        assert(sim_disk_fd);
        for (int i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = (int) fwrite("\0", 1, 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
    }
    // ------------------------------------------------------------------------
    void listAll() {
        int i = 0;
        for (auto it = begin(OpenFileDescriptors); it != end(OpenFileDescriptors); ++it) {
            cout << "index: " << i << ": FileName: " << it -> getFileName() << " , isInUse: " <<
                 it -> isInUse() << " file Size: " << it -> GetFileSize() << endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fread( & bufy, 1, 1, sim_disk_fd);
            cout << bufy;
        }
        cout << "'" << endl;
    }
    // ------------------------------------------------------------------------
    void fsFormat(int blockSize = 4) // a function that formats the disc
    {
        numOfFormat++; // will count the num of times the disc was formatted
        if (blockSize < 1 || blockSize > DISK_SIZE) // the block size is wrong:
        {
            cout << "ERR" << endl;
            return;
        }
        if (numOfFormat == 1) {
            deletedInodes.erase(deletedInodes.begin(), deletedInodes.end()); // resets the vector
        }
        if (numOfFormat > 1) // if it's not the first format
        {
            for (auto & entry: MainDir) // frees the Map
            {
                delete entry.second;
            }
            delete[](BitVector); // frees the Vector
            for (int i = 0; i < DISK_SIZE; i++) {
                int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
                ret_val = (int) fwrite("\0", 1, 1, sim_disk_fd);
                assert(ret_val == 1);
            }
            fflush(sim_disk_fd);
        }
        this -> block_size = blockSize;
        OpenFileDescriptors.erase(OpenFileDescriptors.begin(), OpenFileDescriptors.end()); // resets the vector
        BitVectorSize = DISK_SIZE / blockSize; // calc the number of blocks in the disk
        BitVector = new int[BitVectorSize]; // creating the vector for the free blocks in the disk
        for (int i = 0; i < BitVectorSize; i++) {
            BitVector[i] = 0; // sets all blocks to 0 which means free blocks
        }
        OpenFileDescriptors.clear();
        MainDir.clear(); //resets the map
        is_formated = true;
        freeBlocksNum = DISK_SIZE / blockSize;
    }
    // ------------------------------------------------------------------------
    int CreateFile(string fileName) {
        // checks if we need to format the disk or the file already exist
        if (!is_formated || MainDir.find(fileName) != MainDir.end()) {
            cout << "ERR" << endl;
            return -1;
        }
        auto * myInode = new fsInode(block_size); // creates a new pointer to an Inode
        MainDir.insert({
                               fileName,
                               myInode
                       }); // insert the Inode and his name to the map
        FileDescriptor fd(fileName, myInode);
        for (int i = 0; i < OpenFileDescriptors.size(); i++) {
            if (!OpenFileDescriptors[i].isInUse()) // finding A close file in the vector
            {
                OpenFileDescriptors[i] = fd;
                OpenFileDescriptors[i].setInUse(true);
                return i;
            }
        }
        OpenFileDescriptors.push_back(fd); // insert the new file to the vector
        OpenFileDescriptors[OpenFileDescriptors.size() - 1].setInUse(true);
        return (int) OpenFileDescriptors.size() - 1; // returns the file fd that is his actual index in the vector
    }

    // ------------------------------------------------------------------------
    int OpenFile(string FileName) {
        if (!is_formated || isFileOpen(FileName)) // An open file can't be opened again
        {
            cout << "ERR" << endl;
            return -1;
        }
        auto it = MainDir.find(FileName);
        if (it == MainDir.end()) // file not exist
        {
            cout << "ERR" << endl;
            return -1;
        }
        int j;
        FileDescriptor fd(FileName, MainDir[FileName]); // Creating a new FD
        for (j = 0; j < OpenFileDescriptors.size(); j++) {
            if (!OpenFileDescriptors[j].isInUse()) {
                OpenFileDescriptors[j] = fd;
                OpenFileDescriptors[j].setInUse(true);
                return j;
            }
        }
        //opens the file by adding him to the vector:
        OpenFileDescriptors.push_back(fd);
        return (int) OpenFileDescriptors.size() - 1; // return the file fd

    }

    // ------------------------------------------------------------------------
    string CloseFile(int fd) {
        if (!is_formated || fd < 0 || fd > (OpenFileDescriptors.size() - 1) || !OpenFileDescriptors[fd].isInUse()) {
            cout << "ERR" << endl;
            return "-1";
        }
        OpenFileDescriptors[fd].setInUse(false);
        return OpenFileDescriptors[fd].getFileName();
    }
    // ------------------------------------------------------------------------
    int WriteToFile(int fd, char * buf, int len) {
        if (!is_formated || fd < 0 || fd > (OpenFileDescriptors.size() - 1) || !OpenFileDescriptors[fd].isInUse()) {
            cout << "ERR" << endl;
            return -1;
        }
        int fileRest = OpenFileDescriptors[fd].getInode() -> getFreeFilePlace();
        int rest = (fileRest % block_size); // the place that is in the last block of the inode that is occupied but not full
        int diskRest = rest + (block_size * freeBlocksNum);
        if (diskRest == 0 || fileRest == 0) {
            cout << "ERR" << endl;
            return -1;
        }
        if (len > fileRest || len > (diskRest)) // if the len is bigger than the size left in this dile or in the Disc:
        {
            if (fileRest > diskRest)
                len = diskRest;
            else len = fileRest;
        }
        fsInode * inode = OpenFileDescriptors[fd].getInode(); // saves the current Inode in a fsInode pointer
        int writen = 0, size, currBlock, blockInUse, indSpace, indexBlock, offset;
        while (len > 0) // while we can continue writing
        {
            char * oneBlock = new char[block_size + 1]; // Creating a block_size string + '\0' place
            blockInUse = inode -> getBlockInUse(); // Saving the number of blocks that are in use
            size = offset = block_size;
            if (len < block_size) size = len;
            indSpace = blockSpace(inode);
            if (blockInUse == 0) // the file is empty
            {
                currBlock = findFreeBlock(); // finding a free block in the disk
                if (currBlock == -1) {
                    delete[] oneBlock;
                    break;
                }
                inode -> setDirectBlock1(currBlock);
                inode -> setBlockInUse(inode -> getBlockInUse() + 1);
            } else if (blockInUse == 1) // Direct block 1:
            {
                currBlock = inode -> getDirectBlock1();
                if (indSpace == 0) // if there is no place in 1:
                {
                    currBlock = findFreeBlock();
                    if (currBlock == -1) {
                        delete[] oneBlock;
                        break;
                    }
                    inode -> setDirectBlock2(currBlock);
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                } else offset = indSpace;
            } else if (blockInUse == 2) // Direct block 2:
            {
                currBlock = inode -> getDirectBlock2();
                if (indSpace == 0) // if 2 is full, opening 3:
                {
                    currBlock = findFreeBlock();
                    if (currBlock == -1) {
                        delete[] oneBlock;
                        break;
                    }
                    inode -> setDirectBlock3(currBlock);
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                } else offset = indSpace;
            } else if (blockInUse == 3) // Direct block 3:
            {
                currBlock = inode -> getDirectBlock3();
                if (indSpace == 0) // there is no more space in the direct blocks
                {
                    indexBlock = findFreeBlock();
                    if (indexBlock == -1) {
                        delete[] oneBlock;
                        break;
                    }
                    currBlock = findFreeBlock();
                    if (currBlock == -1) {
                        BitVector[indexBlock] = 0; //frees the block
                        freeBlocksNum++;
                        delete[] oneBlock;
                        break;
                    }
                    // opening a single index block:
                    inode -> setSingleInDirect(indexBlock);
                    writeToInsideIndexBlock(inode, indexBlock, currBlock);
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                    inode -> setNumOfIndexBlocks(inode -> getNumOfIndexBlocks() + 1);
                } else offset = indSpace;
            } else if (blockInUse < (3 + block_size)) // already bigger than 3 for sure.
            {
                indexBlock = inode -> getSingleInDirect();
                currBlock = findLastBlock(inode, indexBlock);
                if (indSpace == 0) // opening a new block in the single indexBlock:
                {
                    currBlock = findFreeBlock();
                    if (currBlock == -1) {
                        delete[] oneBlock;
                        break;
                    }
                    writeToInsideIndexBlock(inode, indexBlock, currBlock);
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                } else offset = indSpace;
            } else if (blockInUse == 3 + block_size) {
                indexBlock = inode -> getSingleInDirect();
                currBlock = findLastBlock(inode, indexBlock);
                if (indSpace == 0) // if the single indexBlock is full
                {
                    indexBlock = findFreeBlock(); // Finding the first index block
                    if (indexBlock == -1) {
                        delete[] oneBlock;
                        break;
                    }
                    int cur = findFreeBlock(); // Finding the second index block
                    if (cur == -1) {
                        BitVector[indexBlock] = 0;
                        freeBlocksNum++;
                        delete[] oneBlock;
                        break;
                    }
                    currBlock = findFreeBlock(); // Finding the real block for the data
                    if (currBlock == -1) {
                        BitVector[cur] = 0;
                        BitVector[indexBlock] = 0;
                        freeBlocksNum = freeBlocksNum + 2;
                        delete[] oneBlock;
                        break;
                    }
                    inode -> setDoubleInDirect(indexBlock); // sets the double IndexBlock
                    writeToOutIndexBlock(inode, indexBlock, cur); // sets his first IndexBlock
                    writeToInsideIndexBlock(inode, cur, currBlock); // sets the first data block in the IndexBlock
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                    inode -> setNumOfIndexBlocks(inode -> getNumOfIndexBlocks() + 2);
                } else offset = indSpace;
            } else // double indirect
            {
                indexBlock = inode -> getDoubleInDirect();
                int lastIndexBlock, lastRealBlock;
                currBlock = findFreeBlock();
                if (currBlock == -1) {
                    delete[] oneBlock;
                    break;
                }
                if ((inode -> getBlockInUse() - 3 - block_size) % block_size == 0) {
                    lastIndexBlock = currBlock;
                    currBlock = findFreeBlock(); // Finding the second index block
                    if (currBlock == -1) {
                        BitVector[lastIndexBlock] = 0;
                        freeBlocksNum++;
                        delete[] oneBlock;
                        break;
                    }
                    // Writes the block to the first index block:
                    writeToInsideIndexBlock(inode, lastIndexBlock, currBlock);
                    inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                    // Writes the first index block to the double indirect block:
                    writeToOutIndexBlock(inode, indexBlock, lastIndexBlock);
                    inode -> setNumOfIndexBlocks(inode -> getNumOfIndexBlocks() + 1);
                } else // if there is still place in the last inside index block:
                {
                    lastIndexBlock = findLastBlock(inode, indexBlock);
                    lastRealBlock = findLastInsideBlock(inode, lastIndexBlock);
                    if (indSpace == 0) {
                        writeToInsideIndexBlock(inode, lastIndexBlock, currBlock);
                        inode -> setBlockInUse(inode -> getBlockInUse() + 1);
                    } else {
                        BitVector[currBlock] = 0;
                        freeBlocksNum++;
                        offset = indSpace;
                        currBlock = lastRealBlock;
                    }
                }
            }
            if (size > offset) {
                size = offset;
            }
            copyStr(oneBlock, buf, offset, writen, 0); //copying the block_size current chars to a string
            writeToBlock(oneBlock, currBlock, size, block_size - offset); //writing them to the Disk file
            len -= size; // decreasing the len
            writen += size; //increase the index in the buf
            inode -> setFileSize(inode -> getFileSize() + size);
            inode -> setFreeFilePlace(inode -> getFreeFilePlace() - size);
        }
        fflush(sim_disk_fd);
        return 1;
    }

    // ------------------------------------------------------------------------
    int DelFile(string FileName) {
        if (!is_formated || isFileOpen(FileName) || MainDir.find(FileName) == MainDir.end()) {
            cout << "ERR" << endl;
            return -1; // if the file is open he cant be deleted
        }
        fsInode * inode = MainDir.find(FileName) -> second;
        int index_block;
        int in_use = inode -> getBlockInUse();
        int freeBlocks = 0;
        // cleans the first 3 Direct blocks:
        cleanBlock(inode -> getDirectBlock1());
        cleanBlock(inode -> getDirectBlock2());
        cleanBlock(inode -> getDirectBlock3());
        freeBlocks = 3;
        if (in_use >= 4) // single indirect block clean:
        {
            index_block = inode -> getSingleInDirect();
            for (int i = 0; i < block_size; i++) {
                if (freeBlocks != in_use) {
                    freeBlocks++;
                    cleanBlock(findInodePlace(inode, freeBlocks));
                }
            }
            cleanBlock(index_block);
        }
        if (in_use >= (block_size + 4)) // Double indirect blocks clean:
        {
            char * Index_blocks = new char[index_block];
            index_block = inode -> getDoubleInDirect();
            readOneBlock(Index_blocks, index_block);
            for (int i = 0; i < block_size; i++) {
                if (freeBlocks != in_use) {
                    for (int j = 0; j < block_size; j++) {
                        if (freeBlocks != in_use) {
                            freeBlocks++;
                            cleanBlock(findInodePlace(inode, freeBlocks));
                        }
                    }
                    cleanBlock((int)(Index_blocks[i]));
                }
            }
            cleanBlock(index_block);
            delete[] Index_blocks;
        }
        // cleans the file in the vector:
        for (int i = 0; i < OpenFileDescriptors.size(); i++) {
            if (OpenFileDescriptors[i].getFileName() == FileName) {
                OpenFileDescriptors[i].getInode() -> setBlockInUse(0);
                OpenFileDescriptors[i].getInode() -> setNumOfIndexBlocks(0);
                OpenFileDescriptors[i].getInode() -> setFileSize(0);
                OpenFileDescriptors[i].setFileName("");
            }
        }
        // clears the file in the map:
        auto it = MainDir.find(FileName);
        fsInode * newInode = it -> second;
        FileDescriptor fd(FileName, newInode);
        deletedInodes.push_back(fd);
        MainDir.erase(it);
        return 1;
    }

    // ------------------------------------------------------------------------
    int ReadFromFile(int fd, char * buf, int len) {
        buf[0] = '\0'; //cleans the input buffer
        if (!is_formated || fd < 0 || fd > (OpenFileDescriptors.size() - 1) || !OpenFileDescriptors[fd].isInUse() || len <= 0) {
            cout << "ERR" << endl;
            return -1;
        }
        if (len > OpenFileDescriptors[fd].getInode() -> getFileSize()) // the len in bigger than the file size
        {
            len = OpenFileDescriptors[fd].getInode() -> getFileSize();
        }
        if (len == 0) return 1;
        int blocksNum = len / block_size; // the integer number of blocks to read
        int currentBlockNum;
        int lastBlock = blocksNum + 1;
        if (len % block_size == 0) // if the last block isn't full:
        {
            lastBlock--;
        }
        for (int i = 1; i <= (lastBlock); i++) // reads the blocks one by one until the last inUse block:
        {
            currentBlockNum = findInodePlace(OpenFileDescriptors[fd].getInode(), i);
            if (currentBlockNum < 0) break; // we arrived to the last block
            char * oneBlock = new char[block_size + 1];
            readOneBlock(oneBlock, currentBlockNum);
            if (i != (blocksNum + 1))
                copyStr(buf, oneBlock, block_size, 0, (i - 1) * block_size);
            else {
                oneBlock[len - (blocksNum * block_size)] = '\0';
                copyStr(buf, oneBlock, block_size, 0, (i - 1) * block_size);
            }
            delete[] oneBlock;
        }
        return 1;
    }
    // ------------------------------------------------------------------------
    int GetFileSize(int fd) {
        if (!is_formated || fd < 0 || fd > (OpenFileDescriptors.size() - 1) || !OpenFileDescriptors[fd].isInUse()) return -1;
        return OpenFileDescriptors[fd].getInode() -> getFileSize();
    }
    // ------------------------------------------------------------------------
    int CopyFile(string srcFileName, string destFileName) {
        if (!is_formated || srcFileName == destFileName || empty(destFileName) || isFileOpen(srcFileName) ||
            MainDir.find(srcFileName) == MainDir.end() || isFileOpen(destFileName)) {
            cout << "ERR" << endl;
            return -1;
        }
        int emptyPlace = (freeBlocksNum * block_size);
        int len = MainDir.find(srcFileName) -> second -> getFileSize();
        int srcFd = -1, destFd = -1;
        if (MainDir.find(destFileName) == MainDir.end()) // dest file don't exists
        {
            if (MainDir.find(srcFileName) -> second -> getFileSize() > emptyPlace) {
                cout << "ERR" << endl;
                return -1;
            }
            destFd = CreateFile(destFileName);
            if (destFd == -1) {
                cout << "ERR" << endl;
                return -1;
            }
            CloseFile(destFd);
        } else // dest file exists
        {
            emptyPlace += MainDir[destFileName] -> getFileSize();
            if (MainDir.find(srcFileName) -> second -> getFileSize() > emptyPlace) {
                cout << "ERR" << endl;
                return -1;
            }
            DelFile(destFileName);
            fsInode * myInode = new fsInode(block_size); // creates a new pointer to an Inode
            MainDir.insert({
                                   destFileName,
                                   myInode
                           }); // insert the Inode and his name to the map
        }
        srcFd = OpenFile(srcFileName);
        if (!isFileOpen(destFileName)) {
            destFd = OpenFile(destFileName);
        }
        char buf[len];
        int val;
        val = ReadFromFile(srcFd, buf, len); // reads the src file
        CloseFile(srcFd);
        if (val == -1) {
            cout << "ERR" << endl;
            return -1;
        }
        val = WriteToFile(destFd, buf, len); // write the src file to the dest file
        if (val == -1) {
            cout << "ERR" << endl;
            return -1;
        }
        CloseFile(destFd);
        return 1;
    }
    // ------------------------------------------------------------------------
    int RenameFile(string oldFileName, string newFileName) {
        if (newFileName == oldFileName) return 1;
        if (!is_formated || MainDir.find(newFileName) != MainDir.end() || MainDir.find(oldFileName) == MainDir.end() ||
            isFileOpen(oldFileName) || newFileName == "") {
            cout << "ERR" << endl;
            return -1;
        }
        auto it = MainDir.find(oldFileName); // finding the old file in the map
        fsInode * newInode = it -> second;
        MainDir.insert({
                               newFileName,
                               newInode
                       }); // opening a new file with the newName
        MainDir.erase(it); // deletes the oldName from the map
        // changing the name in the vector also:
        for (int i = 0; i < OpenFileDescriptors.size(); i++) {
            if (OpenFileDescriptors[i].getFileName() == oldFileName) {
                OpenFileDescriptors[i].setFileName(newFileName);
            }
        }
        return 1;
    }
    // A function that gets a number of a real block,and return his real index in the Disk:
    int findInodePlace(fsInode * inode, int index) {
        int doubleNum = ((block_size * block_size) + (block_size + 4));
        if (index == 1) {
            return inode -> getDirectBlock1();
        } else if (index == 2) {
            return inode -> getDirectBlock2();
        } else if (index == 3) {
            return inode -> getDirectBlock3();
        } else if (index <= (block_size + 3)) //index is already bigger than 3 for sure in this point.
        {
            char IndexBlock[block_size];
            readOneBlock(IndexBlock, inode -> getSingleInDirect());
            return (int) IndexBlock[(index - 3) - 1]; // returns the block number that was asked
        } else if (index < doubleNum) // double indirect
        {
            char IndexBlock1[block_size], IndexBlock2[block_size];
            readOneBlock(IndexBlock1, inode -> getDoubleInDirect());
            int offset = ((index - block_size - 4) / (block_size));
            if (IndexBlock1[offset] != '\0') {
                readOneBlock(IndexBlock2, (int) IndexBlock1[offset]);
                int secondOffSet = (index - (3 + block_size + (block_size * offset))) - 1;
                if (IndexBlock2[secondOffSet] != '\0') {
                    return (int) IndexBlock2[secondOffSet];
                }
            }
        }
        return -1; // when the number of block does not exist.
    }
    // A function that finds a free block in the disk
    int findFreeBlock() {
        for (int i = 0; i < BitVectorSize; i++) {
            if (BitVector[i] == 0) // if the block is free:
            {
                BitVector[i] = 1; // the block is now in use.
                freeBlocksNum--;
                return i; // returns the block number
            }
        }
        return -1; // when there is no free blocks in the disk.
    }
    void readOneBlock(char * block, int index) {
        // Brings the cursor to the right place:
        int val = fseek(sim_disk_fd, (index) * block_size, SEEK_SET);
        if (val != 0) exit(EXIT_FAILURE);
        // Reads the Index Block of the single indirect
        val = (int) fread(block, sizeof(char), block_size, sim_disk_fd);
        if (val != block_size) exit(EXIT_FAILURE);
    }
    void writeToBlock(char * block, int index, int size, int offset) {
        int forward = ((index) * block_size + (offset));
        // Brings the cursor to the right place:
        int val = fseek(sim_disk_fd, forward, SEEK_SET);
        if (val != 0) exit(EXIT_FAILURE);
        // Reads the Index Block of the single indirect
        val = (int) fwrite(block, sizeof(char), size, sim_disk_fd);
        if (val != size) exit(EXIT_FAILURE);
        delete[] block;
    }
    // A function that finds how much place left are in the current existing blocks of the file
    int blockSpace(fsInode * Inode) {
        int totalNum = Inode -> getBlockInUse() * block_size;
        int realNum = Inode -> getFileSize();
        return (totalNum - realNum) % block_size;
    }
    // A function that writes a real block number to an Index block:
    void writeToInsideIndexBlock(fsInode * inode, int indexBlock, int realBlock) {
        int blocks = inode -> getBlockInUse();
        int place;
        if (blocks >= 3 + block_size) {
            place = (blocks - 3 - block_size) % block_size;
        } else place = blocks - 3;
        char * oneBlock = new char[block_size]; // was block_size
        readOneBlock(oneBlock, indexBlock);
        oneBlock[place] = decToBinary(realBlock); // the real block ascii sign
        writeToBlock(oneBlock, indexBlock, place + 1, 0);
    }
    // A function that writes an index block to the double Indirect block:
    void writeToOutIndexBlock(fsInode * inode, int outIndexBlock, int inIndexBlock) {
        int blocks = inode -> getBlockInUse();
        int place = (blocks - 3 - block_size) / block_size;
        char * oneBlock = new char[block_size];
        readOneBlock(oneBlock, outIndexBlock);
        oneBlock[place] = decToBinary(inIndexBlock);
        writeToBlock(oneBlock, outIndexBlock, place + 1, 0);
    }
    // A function that find the last block in the Inode:
    int findLastBlock(fsInode * inode, int indexBlock) {
        int blocks = inode -> getBlockInUse();
        int toReturn, sum;
        char * oneBlock = new char[block_size];
        readOneBlock(oneBlock, indexBlock);
        if (blocks > 3 + block_size) {
            sum = (blocks - 3 - block_size) / block_size;
        } else sum = (blocks - 4) % block_size;
        toReturn = oneBlock[sum];
        delete[] oneBlock;
        return toReturn; //returns the last
    }
    // A function that finds the last real block in a single index block
    int findLastInsideBlock(fsInode * inode, int indexBlock) {
        int blocks = inode -> getBlockInUse();
        int toReturn, sum;
        char * oneBlock = new char[block_size];
        readOneBlock(oneBlock, indexBlock);
        sum = blocks % block_size;
        toReturn = oneBlock[sum];
        delete[] oneBlock;
        return toReturn;
    }
    // A function that copying a source string to a dest string in the desired indexes
    void static copyStr(char * dest, char * source, int size, int from1, int from2) {
        for (int i = 0; i < size; i++) {
            dest[from2 + i] = source[from1 + i];
        }
        dest[from2 + size] = '\0';
    }
    // A function that checks if the file is open or not:
    bool isFileOpen(string fileName) {
        for (auto & OpenFileDescriptor: OpenFileDescriptors) {
            if (OpenFileDescriptor.getFileName() == fileName && OpenFileDescriptor.isInUse())
                return true;
        }
        return false;
    }
    // A function that frees one block in the disk
    void cleanBlock(int index) {
        if (index < 0) return;
        BitVector[index] = 0; // free the Disk block
        freeBlocksNum++; // increase the number of free blocks
    }
    ~fsDisk() // A destructor for fsDisk class:
    {
        for (auto & entry: MainDir) // frees the MainDear Inodes
        {
            delete entry.second;
        }
        for (int i = 0; i < deletedInodes.size(); i++) // frees all the deleted files
        {
            delete deletedInodes[i].getInode();
        }
        delete[](BitVector); // frees the BitVector
        fclose(sim_disk_fd); // closing the disk file
    }
};

int main() {
    int blockSize;
    string fileName;
    string fileName2;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk * fs = new fsDisk();
    int cmd_;
    while (1) {
        cin >> cmd_;
        switch (cmd_) {
            case 0: // exit
                delete fs;
                exit(0);
                break;

            case 1: // list-file
                fs -> listAll();
                break;

            case 2: // format
                cin >> blockSize;
                fs -> fsFormat(blockSize);
                break;

            case 3: // creat-file
                cin >> fileName;
                _fd = fs -> CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4: // open-file
                cin >> fileName;
                _fd = fs -> OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5: // close-file
                cin >> _fd;
                fileName = fs -> CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6: // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs -> WriteToFile(_fd, str_to_write, strlen(str_to_write));
                break;

            case 7: // read-file
                cin >> _fd;
                cin >> size_to_read;
                fs -> ReadFromFile(_fd, str_to_read, size_to_read);
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8: // delete file
                cin >> fileName;
                _fd = fs -> DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 9: // copy file
                cin >> fileName;
                cin >> fileName2;
                fs -> CopyFile(fileName, fileName2);
                break;

            case 10: // rename file
                cin >> fileName;
                cin >> fileName2;
                fs -> RenameFile(fileName, fileName2);
                break;

            default:
                break;
        }
    }
}
