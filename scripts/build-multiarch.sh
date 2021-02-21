docker build -t glpp-arch -f docker/arch/Dockerfile .
docker build -t glpp-ubuntu -f docker/ubuntu/Dockerfile .
docker build -t glpp-debian -f docker/debian/Dockerfile .
docker run glpp-arch test
docker run glpp-ubuntu test
docker run glpp-debian test
