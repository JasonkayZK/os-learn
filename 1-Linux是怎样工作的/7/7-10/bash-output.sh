$ ps ax |grep bash

#10693 ?        S      0:00 /bin/bash /docker-entrypoint.sh
#33708 pts/0    Ss     0:00 -bash
#34890 pts/1    Ss+    0:00 -bash
#36773 pts/0    S+     0:00 grep bash
#50694 ?        Ss     0:00 /bin/bash -c /app/entrypoint.sh

echo abc > /dev/pts/1
