
import os
import sys

f=open('do','w')
ff=open('./tests.fraig/'+sys.argv[1],'r')
rrr=ff.read()
ff.close()
r=rrr.split()
m = int(r[1])+int(r[4])+1
ff.close()
f.write('cirr ./tests.fraig/')
f.write(sys.argv[1] + '\n')
f.write('\ncirp\ncirp -pi\ncirp -po\ncirp -fl\ncirp -n\n')

# for i in range(m):
#         f.write('cirg ')
#         f.write(str(i))
#         f.write(' -fanin 100\n')
#         f.write('cirg ')
#         f.write(str(i))
#         f.write(' -fanout 100\n')

#f.write('cirsw\nciropt\ncirstr\ncirsim -f ./tests.fraig/'+sys.argv[2]+'\n')
#f.write('cirsw\nciropt\ncirstr\n')
f.write('cirsw\nciropt\ncirstr\ncirsim -f ./tests.fraig/'+sys.argv[2]+'\n')
f.write('cirsim -f ./tests.fraig/'+sys.argv[2]+'\n')
f.write('cirp -n\n')
for i in range(m):
        f.write('cirg ')
        f.write(str(i))
        f.write(' -fanin 5\n')
        f.write('cirg ')
        f.write(str(i))
        f.write(' -fanout 5\n')

f.write('cirg 1\ncirg 2\ncirg 3\n')
f.write('cirp\ncirp -pi\ncirp -po\ncirp -fl\ncirp -n\nq -f\n')
#f.write('q -f\n')
f.close()
os.system("./fraig -f do > out1")
os.system("./ref/fraig-mac -f do > out2")
os.system("diff out1 out2")