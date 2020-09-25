This is a pcapng merge service - files in /input is merged and outputted in /output

Allowed options:<br>
  -m [ --mergecount ] arg number of files to merge before creating new file<br>
  -p [ --penalty ] arg    Penalty duration when less then mergecount was found<br>
  -y [ --yield ] arg      yield duration when no files where found<br>

docker-compose.yml<br>
version: '3.3'<br>
services:<br>
  pcapngmerge:<br>
    image: miketruman/pcapngmergeservice:latest<br>
    volumes:<br>
    - /input/:/input/<br>
    - /output/:/output/<br>
    command: /build/a.bin -m 2<br>

