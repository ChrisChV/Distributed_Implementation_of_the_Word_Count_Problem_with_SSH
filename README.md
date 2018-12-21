# Word_Count_Problem_in_Cluster_with_SSH

Implementation of word count problem in a cluster of computers connected with ssh

## Words Generator

The words generators's binary receives the following parameters:

```
$> bin/wordGenerator <wordFile> <OutFileSize(MB)>
```

And an example:

```
$> bin/wordGenerator words/words.txt 10
```

This example uses the words on the file `words/words.txt` to generate a file with random words with size of 10 MB. If you want to test this implementation with a lot of information, you can generate a file with GB of size.

## Parallel word count

The binary `bin/countWords` count words from a file on the local computer in parallel. The binary recieves the following parameters:

```
$> bin/countWords <WordFile> <NumThreads>
```

The algorithm read the file and add the words in a vector. When size of this vector pass a limit, the vector is sended to one thread. This thread counts the words in the vector. When the vector pass the limit again, the vector is sended to another thread.

## Distributed word count

If you want execute the word count in a lot of computers you have cinfigure the `bin/hosts` file as follow:

```
<ip1> <user1> <password1>
<ip2> <user2> <password2>
```

In all of machines you have to copy all files and directories. Then you have to execute the binary `bin/runMain` as follow:

```
$> bin/runMain <fileForCount> <fileOfHosts> <numOfThreadsinMachines>
```




