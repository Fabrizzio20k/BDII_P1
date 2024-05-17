"use client";

import { AppleRecord, AppleRecordKeys } from "./types/appleRecord";
import TableQuery from "./components/TableQuery/TableQuery";
import MessageDisplay from "./components/MessageDisplay/MessageDisplay";
import SpaceQuery from "./components/SpaceQuery/SpaceQuery";
import { useState, useEffect } from "react";
import { MessageDisplayProps } from "./components/MessageDisplay/MessageDisplay";

type parserResponse = {
  command: string;
  condition: string;
  indexColumn: string;
  indexType: string;
  message: string;
  r1: string;
  r2: string;
  route: string;
  status: MessageDisplayProps["status"];
  table: string;
};

const headers: AppleRecordKeys[] = [
  "id",
  "track_name",
  "size_bytes",
  "currency",
  "price",
  "rating_count_tot",
  "rating_count_ver",
  "user_rating",
  "user_rating_ver",
  "ver",
  "cont_rating",
  "prime_genre",
  "sup_devices_num",
  "ipadSc_urls_num",
  "lang_num",
  "vpp_lic",
];

const api_parser = "http://localhost:5000/api/v1/parser";
const api_structures = "localhost:5001/api/v1/parser";

export default function Home() {

  const [created, isCreated] = useState(false);
  const [sent, isSent] = useState(false);
  const [query, setQuery] = useState('');

  const [data, setData] = useState<AppleRecord[]>([]);
  const [response, setResponse] = useState<parserResponse>();

  useEffect(() => {
    if (sent) {
      const data = { "command": query };
      fetch(api_parser, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(data),
      })
        .then((response) => response.json())
        .then((data) => {
          setResponse(data);
          if (data.status === 'ok') {
            switch (data.command) {
              case "CREATE":
                console.log("Table created");
                break;
              case "SELECT":
                console.log("Data selected by one");
                break;
              case "RANGE":
                console.log("Data selected by range");
                break;
              case "INSERTAR":
                console.log("Data inserted");
                break;
              case "BORRAR":
                console.log("Data deleted");
                break;
              default:
                console.log("No command found");
                break;
            }
          }
        })
        .catch((error) => {
          console.error("Error:", error);
        });
      isSent(false);
    }
  }, [sent]);

  return (
    <main className="h-screen w-auto flex flex-col justify-center items-center bg-gray-900 px-20">
      <SpaceQuery isSent={sent} setSent={isSent} query={query} setQuery={setQuery} />
      <div className="w-full flex justify-start items-start">
        {response && <MessageDisplay message={response.message} status={response.status} />}
      </div>
      <div>
        {created && <TableQuery data={data} headers={headers} />}
        {!created && (
          <h2 className="text-white text-2xl font-bold my-36">
            Why not create the table first to see the data? 🤔
          </h2>
        )}
      </div>
    </main>);
}
