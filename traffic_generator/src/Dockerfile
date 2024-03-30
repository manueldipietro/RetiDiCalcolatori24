FROM gcc:latest as builder
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc main.c traffic_generator.c broadcaster.c message.c scl.c sended_recorder.c utility.c traffic_analyzer.c -static -o main

FROM alpine:latest as runtime
COPY --from=builder /usr/src/myapp/main /main
ENTRYPOINT ["./main"]
