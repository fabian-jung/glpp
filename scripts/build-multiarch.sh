docker build -t glpp-arch -f docker/arch/Dockerfile .
docker build -t glpp-ubuntu -f docker/ubuntu/Dockerfile .
docker build -t glpp-debian -f docker/debian/Dockerfile .

docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-arch test
docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-ubuntu test
docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-debian test
