all: build_node_app

build_node_app:
	@echo "Update node binary..." 
	
	cd src/ && \
	npm run clean && \
	npm install && \
	npm run build && \
	cp -r shell/ usb-lockdown/ ../usr/share/hamonize-agent/ && \
	cp -v ./hamonize-agent ../usr/share/hamonize-agent/

clean:
	rm -fv usr/share/hamonize-agent/hamonize-agent && \
	rm -rf usr/share/hamonize-agent/shell/ usr/share/hamonize-agent/usb-lockdown