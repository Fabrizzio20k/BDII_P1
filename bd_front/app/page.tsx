"use client";

import { AppleRecord, AppleRecordKeys } from "./types/appleRecord";
import TableQuery from "./components/TableQuery/TableQuery";
import MessageDisplay from "./components/MessageDisplay/MessageDisplay";
import SpaceQuery from "./components/SpaceQuery/SpaceQuery";
import { useState, useEffect } from "react";
import { MessageDisplayProps } from "./types/messageDisplay";
import axios from "axios";

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
  values: string[];
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
const api_structures = "http://localhost:5001/api/v1/";

export default function Home() {

  const [created, isCreated] = useState(false);
  const [sent, isSent] = useState(false);
  const [query, setQuery] = useState('');
  const [message, setMessage] = useState('');
  const [status, setStatus] = useState<MessageDisplayProps["status"]>('error');
  const [loading, setLoading] = useState(false);

  const [dataRecovered, setDataRecovered] = useState<AppleRecord[]>([]);
  const fetchAll = async (query: string) => {
    setLoading(true);
    try {
      const { data } = await axios.post<parserResponse>(api_parser, { command: query });
      setStatus(data.status);
      setMessage(data.message);

      if (data.status === 'ok') {
        let body = {};
        switch (data.command) {
          case "CREATE":
            body = {
              "file_name": data.route,
              "index": data.indexType,
              "table_name": data.table,
              "column_index": data.indexColumn,
            };
            try {
              const { data } = await axios.post(api_structures + "create", body);
              setMessage(data.message);
              setStatus(data.status);
              if (data.status === 'success' && data.records) {
                setDataRecovered(data.records);
                isCreated(true);
              }
            }
            catch (error) {
              setStatus('error');
              setMessage('Error creating table');
              console.error("Error:", error);
            }
            break;
          case "SELECT":
            body = {
              "search": data.condition
            };
            try {
              const { data } = await axios.post(api_structures + "select", body);
              setMessage(data.message);
              setStatus(data.status);
              if (data.status === 'success' && data.record) {
                let records = JSON.stringify(data.record);
                records = records.replace(/,/g, ",\n");
                setMessage("Record found: " + records);
              }
            }
            catch (error) {
              setStatus('error');
              setMessage('Error selecting record');
              console.error("Error:", error);
            }
            break;
          case "RANGE":
            body = {
              "r1": data.r1,
              "r2": data.r2,
            };
            try {
              const { data } = await axios.post(api_structures + "range", body);
              setMessage(data.message);
              setStatus(data.status);
              if (data.status === 'success' && data.records) {
                setDataRecovered(data.records);
                setMessage("Records found: " + data.records.length + " records. You can see them in the table below.");
              }
              else {
                setDataRecovered([]);
              }
            }
            catch (error) {
              setStatus('error');
              setMessage('Error selecting range');
              console.error("Error:", error);
            }
            break;
          case "INSERTAR":
            body = {
              "record": {
                "id": parseInt(data.values[0]),
                "track_name": data.values[1],
                "size_bytes": parseInt(data.values[2]),
                "currency": data.values[3],
                "price": parseFloat(data.values[4]),
                "rating_count_tot": parseInt(data.values[5]),
                "rating_count_ver": parseInt(data.values[6]),
                "user_rating": parseFloat(data.values[7]),
                "user_rating_ver": parseFloat(data.values[8]),
                "ver": data.values[9],
                "cont_rating": data.values[10],
                "prime_genre": data.values[11],
                "sup_devices_num": parseInt(data.values[12]),
                "ipadSc_urls_num": parseInt(data.values[13]),
                "lang_num": parseInt(data.values[14]),
                "vpp_lic": parseInt(data.values[15]),
              }
            };
            try {
              const { data } = await axios.post(api_structures + "insert", body);
              setMessage(data.message);
              setStatus(data.status);
              if (data.status === 'success' && data.records) {
                setDataRecovered(data.records);
                setMessage("Record inserted with id: " + data.id);
              }
            }
            catch (error) {
              setStatus('error');
              setMessage('Error inserting record');
              console.error("Error:", error);
            }
            break;
          case "BORRAR":
            body = {
              "id": data.condition
            };
            try {
              const { data } = await axios.delete(api_structures + "delete", { data: body });
              setMessage(data.message);
              setStatus(data.status);
              if (data.status === 'success' && data.records) {
                setDataRecovered(data.records);
              }
            }
            catch (error) {
              setStatus('error');
              setMessage('Error deleting record');
              console.error("Error:", error);
            }
            break;
          default:
            setStatus('error');
            setMessage('No command found');
            break;
        }
      }
    }
    catch (error) {
      setStatus('error');
      setMessage('Error parsing query');
      console.error("Error:", error);
    }
    finally {
      setLoading(false);
      isSent(false);
    }
  }
  const handleUpdate = async () => {
    setLoading(true);
    setMessage("Recovering data, please wait...");
    setStatus("success");
    try {
      const { data } = await axios.get(api_structures + "get_all");
      setMessage(data.message);
      setStatus(data.status);
      if (data.status === 'success' && data.records) {
        setDataRecovered(data.records);
      }
    }
    catch (error) {
      setStatus('error');
      setMessage('Error recovering data');
      console.error("Error:", error);
    }
    finally {
      setLoading(false);
    }
  }

  useEffect(() => {
    if (sent) {
      fetchAll(query);
    }
  }, [sent]);

  return (
    <main className="bg-gray-900 relative">
      {loading &&
        (<div className="fixed top-0 left-0 w-full h-full bg-black opacity-75 z-10">
          <div className="fixed top-1/2 left-1/2 border-gray-300 h-20 w-20 animate-spin rounded-full border-8 border-t-blue-500" />
        </div>)
      }
      <div className="min-h-screen w-auto flex flex-col justify-center items-center px-20 overflow-y-auto">
        <SpaceQuery isSent={sent} setSent={isSent} query={query} setQuery={setQuery} />
        <div className="w-full flex justify-start items-start">
          <MessageDisplay message={message} status={status} />
        </div>
        <div className="overflow-x-auto w-11/12">
          {created && <TableQuery data={dataRecovered} headers={headers} />}
          {!created && (
            <h2 className="text-white text-2xl font-bold my-36">
              Why not create the table first to see the data? 🤔
            </h2>
          )}
        </div>
        {created && (
          <button
            className="my-8 bg-green-800 hover:bg-green-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline"
            onClick={handleUpdate}>
            Update Table Data
          </button>
        )}
      </div>
    </main>
  );
}
