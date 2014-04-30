from twilio.rest import TwilioRestClient

# Your Account Sid and Auth Token from twilio.com/user/account
account_sid = "ACc847b79a77bfd43deae56f8fbda4b1d2"
auth_token  = "9337c2c95e8cd5bdf99365c33fe8a784"
client = TwilioRestClient(account_sid, auth_token)
message = client.sms.messages.create(body="No movement observed",
to="+919820726109",    # Replace with your phone number
from_="+17577850843") # Replace with your Twilio number
print "Message sent"
