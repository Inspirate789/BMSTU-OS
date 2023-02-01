FAKEROOT=$HOME/fakeroot
mkdir -p $FAKEROOT
mkdir -p $FAKEROOT/{bin,lib64,lib}
cd $FAKEROOT
cp -v /bin/{bash,ls} $FAKEROOT/bin
list="$(ldd /bin/bash | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp  -v "$i" "${FAKEROOT}${i}"; done
list="$(ldd /bin/ls | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp  -v "$i" "${FAKEROOT}${i}"; done
sudo chroot $FAKEROOT /bin/bash
