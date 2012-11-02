import smtplib, os
#from getpass import getpass
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.Utils import COMMASPACE, formatdate
from email import Encoders

def send_mail():
    #This version promts the user to specify recipient, files,
    #subject, and message. The version for incorperating in the
    #notifier will need to read from a preferences file that 
    #should be modifiable by the user.

    send_to = raw_input("Send To (Separated by a comma): ")
    files = raw_input("Files (Separated by a comma): ")

    send_to=send_to.split(',')
    files=filter(None, files.split(','))

    #Make sure that we are working with a list
    assert type(send_to)==list
    assert type(files)==list

    send_from = raw_input("Send From: ")
    subject = raw_input("Subject: ")

    #Create a message object that we will be sending
    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    text = raw_input("Message: ")

    msg.attach( MIMEText(text) )

    #Attach files to message
    for f in files:
        part = MIMEBase('application', "octet-stream")
        part.set_payload( open(f,"rb").read() )
        Encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="%s"' % os.path.basename(f))
        msg.attach(part)

    #Open up a connection to the gmail servers on port 587
    server = smtplib.SMTP('smtp.gmail.com',587) #port 465 or 587
    server.ehlo()
    server.starttls()
    server.ehlo()
    #password = getpass()
    password="beagleece497"
    server.login(send_from, password)
    server.sendmail(send_from,",".join(send_to),msg.as_string())
    server.close()



if __name__ == '__main__':
    send_mail()
