import { MessageDisplayProps } from "@/app/types/messageDisplay";

export default function MessageDisplay({ message, status }: MessageDisplayProps) {
    return (
        <div className="w-full flex flex-col items-center justify-center mb-8">
            <div className="w-full">
                <div className="flex flex-col shadow-md rounded">
                    {status === 'error' ? (
                        <div className="shadow appearance-none border rounded w-full py-2 px-3 leading-tight focus:outline-none focus:shadow-outline resize-none bg-zinc-950" style={{ fontFamily: '"Fira Code", monospace' }}>
                            <span className="text-gray-200 ">Response: </span> <span className="text-red-500">{message}</span> 😢
                        </div>) : (
                        <div className="shadow appearance-none border rounded w-full py-2 px-3 leading-tight focus:outline-none focus:shadow-outline resize-none bg-zinc-950" style={{ fontFamily: '"Fira Code", monospace' }}>
                            <span className="text-gray-200 ">Response: </span> <span className="text-green-500">{message}</span> 😃
                        </div>)}
                </div>
            </div>
        </div>
    );
}
