FROM alpine:latest AS builder
COPY . /app/src
WORKDIR /app/build
RUN apk add cmake build-base git mbedtls-static mbedtls-dev ninja
RUN CMAKE_FIND_LIBRARY_SUFFIXES=".a" CXXFLAGS="-s -Oz" CFLAGS="-s" CC="gcc" CXX="g++"  \
    cmake -DUSE_MBED_TLS:BOOL=ON -DCMAKE_BUILD_TYPE=MinSizeRel -G Ninja -DUSE_ZLIB:BOOL=OFF -DCMAKE_EXE_LINKER_FLAGS="-static" ../src \
    -DMBEDTLS_LIBRARY=/usr/lib/libmbedtls.a \
      -DMBEDCRYPTO_LIBRARY=/usr/lib/libmbedcrypto.a \
      -DMBEDX509_LIBRARY=/usr/lib/libmbedx509.a \
    && cmake --build .

FROM scratch
COPY --from=builder /app/build/webhook .
CMD ["/webhook"]
