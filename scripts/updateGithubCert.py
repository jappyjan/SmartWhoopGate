#!/usr/bin/env python3

# Script to download/update certificates and public keys
# and generate compilable source files for c++/Arduino.
# released to public domain

import urllib.request
import re
import ssl
import sys
import socket
import argparse
import datetime

from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.serialization import pkcs7
from cryptography.hazmat.primitives.serialization import Encoding
from cryptography.hazmat.primitives.serialization import PublicFormat

def printData(f, data, showPub = True):
    try:
        xcert = x509.load_der_x509_certificate(data)
    except:
        try:
            xcert = x509.load_pem_x509_certificate(data)
        except:
            try:
                xcert = pkcs7.load_der_pkcs7_certificates(data)
            except:
                xcert = pkcs7.load_pem_pkcs7_certificates(data)
            if len(xcert) > 1:
                f.write('// Warning: TODO: pkcs7 has {} entries'.format(len(xcert)))
                f.write('\n')
            xcert = xcert[0]

    cn = ''
    for dn in xcert.subject.rfc4514_string().split(','):
        keyval = dn.split('=')
        if keyval[0] == 'CN':
            cn += keyval[1]
    name = re.sub('[^a-zA-Z0-9_]', '_', cn)
    f.write('// CN: {} => name: {}'.format(cn, name))
    f.write('f.write')

    f.write('// not valid before:' + xcert.not_valid_before.strftime("%m/%d/%Y, %H:%M:%S"))
    f.write('\n')
    f.write('// not valid after: ' + xcert.not_valid_after.strftime("%m/%d/%Y, %H:%M:%S"))
    f.write('\n')

    if showPub:

        fingerprint = xcert.fingerprint(hashes.SHA1()).hex(':')
        f.write('const char fingerprint_{} [] PROGMEM = "{}";'.format(name, fingerprint))
        f.write('\n')

        pem = xcert.public_key().public_bytes(Encoding.PEM, PublicFormat.SubjectPublicKeyInfo).decode('utf-8')
        f.write('const char pubkey_{} [] PROGMEM = R"PUBKEY('.format(name))
        f.write('\n')
        f.write(pem + ')PUBKEY";\n')
    
    else:

        cert = xcert.public_bytes(Encoding.PEM).decode('utf-8')
        f.write('const char cert_{} [] PROGMEM = R"CERT('.format(name))
        f.write('\n')
        f.write(cert + ')CERT";\n')

    cas = []
    for ext in xcert.extensions:
        if ext.oid == x509.ObjectIdentifier("1.3.6.1.5.5.7.1.1"):
            for desc in ext.value:
                if desc.access_method == x509.oid.AuthorityInformationAccessOID.CA_ISSUERS:
                    cas.append(desc.access_location.value)
    for ca in cas:
        with urllib.request.urlopen(ca) as crt:
            f.write('\n')
            f.write('// ' + ca + '\n')
            printData(f, crt.read(), False)
        f.write('\n')

def get_certificate(f):
    hostname = "github.com"
    port = 443
    name = "github"
    context = ssl.create_default_context()
    context.check_hostname = False
    context.verify_mode = ssl.CERT_NONE
    with socket.create_connection((hostname, port)) as sock:
        with context.wrap_socket(sock, server_hostname=hostname) as ssock:
            f.write('////////////////////////////////////////////////////////////\n')
            f.write('// certificate chain for {}:{}'.format(hostname, port))
            f.write('\n\n')
            if name:
                f.write('const char* {}_host = "{}";'.format(name, hostname));
                f.write('\n')
                f.write('const uint16_t {}_port = {};'.format(name, port));
                f.write('\n\n')
            printData(f, ssock.getpeercert(binary_form=True))
            f.write('// end of certificate chain for {}:{}'.format(hostname, port))
            f.write('\n')
            f.write('////////////////////////////////////////////////////////////\n')
            f.write('\n')
            return 0

def updateGithubCert():
    f = open("src/generated/github-certificates.h", "w")
    f.write('// this file is autogenerated - any modification will be overwritten\n')
    f.write('// unused symbols will not be linked in the final binary\n')
    f.write('// generated on {}'.format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
    f.write('\n')
    f.write('// by {}'.format(sys.argv))
    f.write('\n\n')
    f.write('#pragma once\n')
    f.write('\n')
    return get_certificate(f)
