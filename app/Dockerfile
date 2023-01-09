FROM tsl0922/ttyd


WORKDIR /WORKSPACE

COPY . /WORKSPACE

RUN apt update && \
	apt install -y \
		libncurses5 \
		--no-install-recommends ntp

ENTRYPOINT ["ttyd", "-p" ,"7000", "-t", "fontSize=30" ,"/WORKSPACE/puyo_client"]
