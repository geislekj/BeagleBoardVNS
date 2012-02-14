import smtplib, os
#from getpass import getpass
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.Utils import COMMASPACE, formatdate
from email import Encoders

def send_mail():

    send_to = raw_input("Send To (Separated by a comma): ")
    files = raw_input("Files (Separated by a comma): ")

    send_to=send_to.split(',')
    files=filter(None, files.split(','))

    assert type(send_to)==list
    assert type(files)==list

    send_from = raw_input("Send From: ")
    subject = raw_input("Subject: ")

    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    text = raw_input("Message: ")

    msg.attach( MIMEText(text) )

    for f in files:
        part = MIMEBase('application', "octet-stream")
        part.set_payload( open(f,"rb").read() )
        Encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="%s"' % os.path.basename(f))
        msg.attach(part)

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
