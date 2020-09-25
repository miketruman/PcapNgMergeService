# PcapNgMergeService

This is a pcapng merge service - files in /input is merged and outputted in /output

Allowed options:
```python
  -m [ --mergecount ] arg number of files to merge before creating new file
  -p [ --penalty ] arg    Penalty duration when less then mergecount was found
  -y [ --yield ] arg      yield duration when no files where found
```

#docker-compose.yml
```python
version: '3.3'
services:
  pcapngmerge:
    image: miketruman/pcapngmergeservice:latest
    volumes:
    - /input/:/input/
    - /output/:/output/
    command: /build/a.bin -m 2
```
