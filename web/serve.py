import http.server
import ssl
import socket
import os
import datetime
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization
import ipaddress

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip

PORT = 8443
web_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(web_dir)

cert_file = os.path.join(web_dir, "cert.pem")
key_file = os.path.join(web_dir, "key.pem")
local_ip = get_local_ip()

# Gera certificado SSL auto-assinado válido se não existir
if not os.path.exists(cert_file) or not os.path.exists(key_file):
    print("[*] Gerando certificado SSL nativo para HTTPS / Web Bluetooth...")
    key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    
    subject = issuer = x509.Name([
        x509.NameAttribute(NameOID.COMMON_NAME, local_ip),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, "BikeComputer"),
    ])
    
    cert = x509.CertificateBuilder().subject_name(
        subject
    ).issuer_name(
        issuer
    ).public_key(
        key.public_key()
    ).serial_number(
        x509.random_serial_number()
    ).not_valid_before(
        datetime.datetime.now(datetime.timezone.utc)
    ).not_valid_after(
        datetime.datetime.now(datetime.timezone.utc) + datetime.timedelta(days=365)
    ).add_extension(
        x509.SubjectAlternativeName([
            x509.IPAddress(ipaddress.IPv4Address(local_ip)),
            x509.DNSName("localhost")
        ]),
        critical=False,
    ).sign(key, hashes.SHA256())

    with open(key_file, "wb") as f:
        f.write(key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        ))
        
    with open(cert_file, "wb") as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))
    print("[V] Certificado SSL gerado com sucesso!")

class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        super().end_headers()

server = http.server.HTTPServer(('0.0.0.0', PORT), Handler)
context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(certfile=cert_file, keyfile=key_file)
server.socket = context.wrap_socket(server.socket, server_side=True)

print(f"\n==================================================================")
print(f"  [V] SERVIDOR HTTPS ATIVO PARA O WEB BLUETOOTH")
print(f"------------------------------------------------------------------")
print(f"  No Chrome do seu Android (conectado no mesmo Wi-Fi), acesse:")
print(f"  --> https://{local_ip}:{PORT}")
print(f"")
print(f"  * Como o certificado e local, o Chrome vai mostrar 'Nao seguro'.")
print(f"  * Basta clicar em 'Avancado' -> 'Continuar para {local_ip}'.")
print(f"==================================================================\n")

try:
    server.serve_forever()
except KeyboardInterrupt:
    print("\nServidor encerrado.")
