import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "Index Experiment with AVL, Extendible Hashing, and B+ Trees",
  description: "Index Experiment with AVL, Extendible Hashing, and B+ Trees",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="es" className="dark text-foreground bg-background">
      <body className={inter.className}>{children}</body>
    </html>
  );
}
