# Update project with:
# $(ANDROID_SDK)/tools/android update project --name AsteroidsVR --target android-23 --path .

PROTOBUF_JAR = protobuf-javanano-3.0.0-alpha-7.jar


all: libs/$(PROTOBUF_JAR) libs/armeabi-v7a/libgvr.so libs/armeabi-v7a/libasteroidsvr.so
	ant -Dgvr.sdk=$(GVRSDK) -emacs debug

libs/$(PROTOBUF_JAR):
	wget -O $@ http://central.maven.org/maven2/com/google/protobuf/nano/protobuf-javanano/3.0.0-alpha-7/$(PROTOBUF_JAR)

libs/armeabi-v7a/libgvr.so:
	mkdir -p libs/armeabi-v7a
	cp -f $(GVRNDK)/lib/android_arm/libgvr.so libs/armeabi-v7a/libgvr.so

libs/armeabi-v7a/libasteroidsvr.so: ../cxx/asteroidsvr.so
	mkdir -p libs/armeabi-v7a
	cp -f ../cxx/asteroidsvr.so libs/armeabi-v7a/libasteroidsvr.so

clean:
	ant clean
	rm -rf res.generated

.PHONY: clean all
