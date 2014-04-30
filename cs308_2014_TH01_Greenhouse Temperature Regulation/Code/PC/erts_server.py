import threading
import webbrowser
import BaseHTTPServer
import SimpleHTTPServer
import serial
import time

ser = serial.Serial(11) # serial where to write
FILE = 'erts_client.html' # url to accept requests
PORT = 9000 # port number for the server

class RequestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
	'''
		Handles HTTP web requests and inherits "SimpleHTTPRequestHandler"
	'''
	postdata = dict()

	def do_POST(self):
		'''
			Handles POST request
		'''
		length = int(self.headers.getheader('content-length'))
		data_string = self.rfile.read(length) # read post data as URL encoded string

		for datapoint in data_string.split('&'):
			# this loop splits stringified post data and stores has key-value pairs
			datapoint = datapoint.split('=')
			self.postdata[datapoint[0]] = datapoint[1]

		print self.postdata # print on server terminal for debugging purposes

		try:
			# now take action according to inputs and get output
			result = process_post_data(self.postdata)
		except:
			result = 'Error'

		self.wfile.write(result) # display output

def sanitize_and_send(threshold1,threshold2,temp_constant):
	# check if inputs are in desired format

	flag = 1 # flag will be set to 0 if something is wrong
	result = ""
	print threshold1,threshold2,temp_constant
	
	if(threshold1 > 50 or threshold1<10):
		result += "Threshold Temperature 1 should be between 10 and 50\n"
		flag = 0
	
	if(threshold2>50 or threshold2<10):
		result += "Threshold Temperature 2 should be between 10 and 50\n"
		flag = 0
	
	if(threshold2<threshold1):
		result += "Threshold Temperature 2 should be greater than Threshold Temperature 1\n"
		flag = 0
	
	if(temp_constant<1 or temp_constant>9):
		result += "Temperature Buffer should be between 1 and 9\n"
		flag = 0
	
	if (flag): # if flag == 1, then aal iz well
		# write data to serial (this serial communicates with our bot)
		ser.write(str(threshold1))
		ser.write(str(threshold2))
		ser.write(str(temp_constant))
		result += "Values sent successfully\n"
	return result
        
def process_post_data(postdata):
	# processes the inputs
	print postdata # for debugging
	result = ""
	try:
		t1 = postdata['threshold1']
		t2 = postdata['threshold2']
		tb = postdata['tbuff']
		
		# convert string to int
		t1 = int(t1)
		t2 = int(t2)
		tb = int(tb)
		
		# sanitize input and send
		result = sanitize_and_send(t1,t2,tb)
	except Exception as e:
		result = e
	return result

def open_browser():
	# Start a browser after waiting for half a second.
	def _open_browser():
		webbrowser.open('http://localhost:%s/%s' % (PORT, FILE))
	thread = threading.Timer(0.5, _open_browser)
	thread.start()

def start_server():
	# Start the server.
	server_address = ("", PORT)
	server = BaseHTTPServer.HTTPServer(server_address, RequestHandler)
	server.serve_forever()

if __name__ == "__main__":
	open_browser()
	start_server()
