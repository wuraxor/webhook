FROM alpine:latest AS builder
COPY . /app/src
WORKDIR /app/build
RUN apk add cmake clang git openssl-dev openssl-libs-static ninja
RUN CXXFLAGS="-s" CFLAGS="-s" cmake -DOPENSSL_USE_STATIC_LIBS=TRUE -DCMAKE_BUILD_TYPE=MinSizeRel -G Ninja -DUSE_ZLIB:BOOL=OFF -DCMAKE_EXE_LINKER_FLAGS="-static" ../src && cmake --build .

FROM gcr.io/distroless/static-debian12
COPY --from=builder /app/build/webhook .
CMD ["/webhook"]