docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-arch test
docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-ubuntu test
docker run --mount src="$(pwd)",target=/glpp,type=bind,readonly glpp-debian test
